#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <log4cpp/Category.hh>
#include <stdexcept>
#include <string>
#include <vector>

#include "json/value.h"
#include "json/writer.h"

#include "server/backup/backup.h"
#include "server/configure/configure.h"
#include "server/crypto/crypto.h"
#include "server/deflate/zip.h"
#include "server/pack/pack.h"
#include "server/tools/committable.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {
/**
 * @brief 前端的直接调用接口，处理整个commit流程
 *
 * @param files 一个文件路径数组，要求是绝对路径
 *
 * @param cr 一个CommitRecord的json对象
 *
 * @param key 加密密钥
 *
 */
void backupFiles(
  const std::vector<fs::path>& files,
  CommitTableRecord& cr,
  const std::string& key
) {
  log4cpp::Category::getRoot().infoStream()
    << "Backup started, log messeag is \"" << cr.message << "\" at "
    << cr.createTime;

  cr.encodedKey = Cryptor::encodeKey(key);

  // 检查是否提交过
  // 如果没有提交，那么会先在内存中添加本次commit
  if (CommitTable::isCommitted(cr)) {
    throw std::runtime_error(
      "Commit log is already committed, its uuid is " + cr.uuid
    );
  }

  // 创建输出目录
  std::string path = cr.storagePath + "/" + cr.uuid;
  fs::path dir = fs::path(path);

  if (!fs::exists(dir)) {
    fs::create_directories(dir);
  }

  // 打开输出流
  path += "/" + cr.uuid;
  std::ofstream outputFile(path, std::ios::binary);
  if (!outputFile) {
    throw std::runtime_error("Failed to open: " + path);
  }

  // 获取全部文件的最近公共祖先
  fs::path lca = getCommonAncestor(files);

  // 获取是否加密
  bool encrypt = cr.isEncrypt;

  // 实例化CRC类和CRC校验码
  CRC crc;
  std::vector<uint8_t> checksum(CRC32::DIGESTSIZE, 0);

  outputFile.write(
    reinterpret_cast<const char*>(checksum.data()),
    static_cast<std::streamsize>(checksum.size())
  );

  // 实例化加密IV
  std::array<CryptoPP::byte, AES::BLOCKSIZE> iv{};

  // 实例化加密类
  Cryptor encryptor(key);

  if (encrypt) {
    // 如果需要加密
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock(iv.data(), iv.size());

    crc.update(std::vector<uint8_t>(iv.data(), iv.data() + iv.size()));

    // 把IV写入文件开头，这部分不需要压缩和加密
    outputFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());
  }

  // 实例化压缩类
  Zip zip;

  zip.init_worker();

  // 初始化循环条件
  bool flush = false;

  // 主循环
  try {
    while (true) {
      std::vector<uint8_t> zippedData{};
      std::vector<uint8_t> processedData{};

      // 获取输出
      auto [packFlush, packedData] = packFilesByBlock(files, flush, lca);

      if (packFlush) {
        // fill input to zip
        zip.fill_input(packedData, flush);

        // if zip is full or flush, then try to get the output
        while (zip.full() || flush) {
          // get output from zip blockedly
          auto zipped_data = zip.get_output(true);

          // if zipped_data is valid, append it to zippedData and try to get
          // more output
          while (zipped_data.valid) {
            // append zipped data to zippedData
            zippedData.insert(
              zippedData.end(), zipped_data.data.begin(), zipped_data.data.end()
            );

            // if eof, break
            if (zipped_data.eof) {
              break;
            }

            // if flush, get output with block
            zipped_data = zip.get_output(flush);
          }

          // if eof, break
          if (zipped_data.eof) {
            break;
          }
        }

        packedData.clear();
      }

      // 当zippedData不为空
      if (!zippedData.empty()) {
        // 如果需要加密，此时则将压缩后的processedData加密
        while (encrypt) {
          auto [encryptFlush, encryptLack, outputData] =
            encryptor.encryptFile(zippedData, iv, flush);

          if (encryptFlush) {
            processedData.insert(
              processedData.end(), outputData->begin(), outputData->end()
            );

            outputData->clear();
          }

          if (encryptLack) {
            break;
          }
        }

        if (!encrypt) {
          processedData = std::move(zippedData);
        }

        // 更新CRC
        crc.update(processedData);

        // 写入文件
        outputFile.write(
          reinterpret_cast<const char*>(processedData.data()),
          static_cast<std::streamsize>(processedData.size())
        );
      }

      // 如果所有文件都读取完毕，设置 flush 为 true
      // 下一次循环会将pack的obuffer强制输出，全部加入zip的ibuffer，并且zip也会强制输出obuffer
      // (pack的obuffer是zip的ibuffer的二分之一，并且有循环读机制，因此不需要担心溢出，就算溢出也会全部写入processedData)
      // 强制输出后，packFlush为true，flush也是true，此时可以退出
      if (!packFlush) {
        flush = true;
      }

      // 退出循环条件
      if (packFlush && flush) {
        break;
      }
    }

    // 获取CRC校验码并写入文件
    checksum = crc.getChecksum();
    outputFile.seekp(0);
    outputFile.write(
      reinterpret_cast<const char*>(checksum.data()),
      static_cast<std::streamsize>(checksum.size())
    );

  } catch (std::exception& e) {
    // 移除失败文件
    fs::remove_all(dir);
    // 移除commit
    CommitTable::removeCommitRecord(cr.uuid);
    outputFile.close();

    throw std::runtime_error(
      "Error occurred when trying to pack files, its uuid is " + cr.uuid +
      ", because " + std::string(e.what())
    );
  }

  // 生成目录文件
  try {
    writeDirectoryFile(fs::path(dir) / "directoryfile", files, lca);
  } catch (std::exception& e) {
    // 移除失败文件
    fs::remove_all(dir);
    // 移除commit
    CommitTable::removeCommitRecord(cr.uuid);
    outputFile.close();

    throw std::runtime_error(
      "Error occurred when trying to write directory file, its uuid is " +
      cr.uuid + ", because " + std::string(e.what())
    );
  }

  // 完成后添加到CommitTable
  try {
    CommitTable::writeCommitTable(COMMIT_TABLE_PATH);
  } catch (const std::exception& e) {
    // 移除失败文件
    fs::remove_all(dir);
    // 移除commit
    CommitTable::removeCommitRecord(cr.uuid);
    outputFile.close();

    throw std::runtime_error(
      "Error occurred when trying to append commit log, its uuid is " +
      cr.uuid + ", because " + std::string(e.what())
    );
  }
}

/**
 * @brief 获取一个绝对路径数组的最近公共父目录
 *
 * @param paths 绝对路径数组
 *
 */
fs::path getCommonAncestor(const std::vector<fs::path>& paths) {
  if (paths.empty()) {
    throw std::runtime_error("Paths array is empty");
  }

  if (paths.size() == 1) {
    return paths[0].filename();
  }

  // 初始化公共祖先为第一个路径
  fs::path commonAncestor = paths[0];

  for (const auto& path : paths) {
    fs::path tempAncestor;
    auto it1 = commonAncestor.begin();
    auto it2 = path.begin();

    // 比较路径的每一部分，找到公共部分
    while (it1 != commonAncestor.end() && it2 != path.end() && *it1 == *it2) {
      tempAncestor /= *it1;
      ++it1;
      ++it2;
    }

    commonAncestor = tempAncestor;
  }

  return commonAncestor;
}

/**
 * @brief 物理删除给定uuid的commit所对应的文件
 *
 * @param uuid 给定的uuid
 *
 */
void removeCommitById(const std::string& uuid) {
  CommitTableRecord ctr = CommitTable::getCommitRecordById(uuid);

  if (!ctr.isDelete) {
    throw std::runtime_error("Commit " + uuid + " is not deleted yet");
  }

  fs::path path = ctr.storagePath + "/" + uuid;

  if (fs::exists(path)) {
    try {
      fs::remove_all(path);
    } catch (std::exception& e) {
      throw std::runtime_error(
        "Cannot remove storage file at " + path.string() + e.what()
      );
    }
  }

  // nothing...
}

/**
 * @brief 物理删除给定uuid的CommitRecord，以及其所对应的文件
 *
 * @param uuid 给定的uuid
 *
 */
void removeCommitAndRecordById(const std::string& uuid) {
  log4cpp::Category::getRoot().infoStream()
    << "Removing commit files by given uuid: " << uuid;

  // 尝试物理删除文件
  try {
    removeCommitById(uuid);
  } catch (std::exception& e) {
    throw std::runtime_error(
      "Error occur when trying to remove commit files by given uuid: " + uuid +
      ", because " + e.what()
    );
  }

  // 尝试从commit_table里删除记录
  try {
    CommitTable::removeCommitRecord(uuid);
    CommitTable::writeCommitTable(COMMIT_TABLE_PATH);
  } catch (std::runtime_error& e) {
    throw std::runtime_error(
      "Error occur when trying to remove commit, because " +
      std::string(e.what()) +
      " (but its storage files already removed or moved)"
    );
  }
}

/**
 * @brief 给定一个路径，生成一个描述Commit内容的目录树文件(Json形式)
 *
 * @param dst 指定的目录文件路径
 *
 * @param files 文件的绝对路径数组
 *
 * @param lca 这些文件的最近公共父路径
 *
 */
void writeDirectoryFile(
  const fs::path& dst,
  const std::vector<fs::path>& files,
  const fs::path& lca
) {
  if (fs::exists(dst)) {
    throw std::runtime_error("file: " + dst.string() + " already exists");
  }
  std::ofstream outFile(dst);

  Json::Value root;
  root["data"] = Json::arrayValue;

  for (const auto& path : files) {
    if (fs::is_socket(path)) {
      continue;
    }

    FileDetail fd = getFileDetail(path);
    Json::Value temp;

    fs::path filePath = fs::relative(path.parent_path(), lca);
    filePath = filePath / path.filename();

    temp["type"] = static_cast<int>(fd.type);  // 将文件类型转换为整数
    temp["createTime"] = fd.createTime;
    temp["updateTime"] = fd.updateTime;
    temp["size"] = static_cast<Json::Int64>(fd.size);  // 确保大小是64位整数
    temp["owner"] = fd.owner;
    temp["group"] = fd.group;
    temp["mode"] = fd.mode;
    temp["relativePath"] = filePath.string();
    temp["dev"] = fd.dev;

    root["data"].append(temp);
  }

  Json::StreamWriterBuilder writer;
  outFile << Json::writeString(writer, root);
  outFile.close();
}

}  // namespace zipfiles::server
