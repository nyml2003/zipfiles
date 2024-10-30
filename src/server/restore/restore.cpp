#include "server/restore/restore.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <log4cpp/Category.hh>
#include <stdexcept>
#include <string>
#include <vector>
#include "json/reader.h"
#include "json/value.h"
#include "server/configure/configure.h"
#include "server/crypto/crypto.h"
#include "server/deflate/zip.h"
#include "server/pack/unpack.h"
#include "server/tools/committable.h"

namespace zipfiles::server {

/**
 * @brief 将指定的commitlog恢复到指定的目录
 *
 * @param dst 指定的恢复路径
 *
 * @param cl 给定的commitlog(json形式)
 *
 * @param key 给定的解密密钥
 */
void restoreTo(
  const fs::path& dst,
  const std::string& uuid,
  const std::string& key
) {
  log4cpp::Category::getRoot().infoStream()
    << "Restore started, log uuid is " << uuid << ", to " << dst;

  CommitTableRecord cr = CommitTable::getCommitRecordById(uuid);

  if (cr.isDelete) {
    throw std::runtime_error("Commit is deleted, its uuid is " + uuid);
  }

  // 检查目标路径是否存在，如果不存在则创建目录
  if (!fs::exists(dst)) {
    fs::create_directories(dst);
  }

  // 打开输入流
  std::string filePath = cr.storagePath + "/" + uuid + "/" + uuid;

  // 校验CRC
  if (!CRC::check(filePath)) {
    throw std::runtime_error("CRC check failed, its uuid is " + uuid);
  }

  std::ifstream inFile(filePath, std::ios::binary);
  if (!inFile) {
    throw std::runtime_error("Failed to open: " + filePath);
  }

  inFile.seekg(CRC32::DIGESTSIZE);

  // 如果有加密，则读取IV
  bool decrypt = cr.isEncrypt;
  std::array<CryptoPP::byte, AES::BLOCKSIZE> iv{};
  if (decrypt) {
    inFile.read(reinterpret_cast<char*>(iv.data()), iv.size());
  }

  // 实例化解码器
  Cryptor decryptor(key);

  // 实例化解包器
  FileUnpacker fileUnpacker(dst);

  Unzip unzip;

  // 读取备份文件
  try {
    std::vector<uint8_t> buffer(PACK_BLOCK_SIZE);
    std::vector<uint8_t> decryptedData{};
    std::vector<uint8_t> unzippedData{};

    bool flush = false;

    while (inFile.read(
             reinterpret_cast<char*>(buffer.data()),
             static_cast<std::streamsize>(buffer.size())
           ) ||
           inFile.gcount() >= 0) {
      // 从备份文件不断读，尝试读满buffer
      // 如果buffer没有读满，或者刚好读到0，那么说明没有更多的数据了
      size_t bytesRead = inFile.gcount();
      // 更改buffer size为实际读取的大小
      buffer.resize(bytesRead);

      // 最后一次读取，使flush为true
      if (buffer.size() < buffer.capacity()) {
        flush = true;
      }

      if (decrypt) {
        // 需要解密
        while (true) {
          // 将buffer数据加入decrypt的ibuffer
          // 当flush为假时，decrypt会检查ibuffer的数据是否足够，如果足够则解密，并使decryptFlush为真
          // 有可能buffer足够使ibuffer填满，但是buffer还有剩余，采用一个无限循环来保证消费完buffer
          // flush为真则强制输出
          auto [decryptFlush, decryptLack, outputData] =
            decryptor.decryptFile(buffer, iv, flush);

          if (decryptFlush) {
            // decrypt输出
            // 将数据加入到decryptedData
            // decryptedData大小实际上由buffer决定，只要没有消费完buffer，那么就一直往其中填入数据
            decryptedData.insert(
              decryptedData.end(), outputData->begin(), outputData->end()
            );

            // 清空outputData
            outputData->clear();
          }
          if (decryptLack) {
            // 如果decrypt消费完了buffer，那么其会使lack为真，从循环中退出
            // 当flush为真时，decryptLack也会为真，此时强制退出
            break;
          }
        }
      } else {
        decryptedData = buffer;
      }

      // 对所有decryptedData解压缩
      unzip.reset_input(&decryptedData);
      while (true) {
        auto [done, lack, outputData] = unzip.run();
        if (done) {
          unzippedData.insert(
            unzippedData.end(), outputData->begin(), outputData->end()
          );
        } else if (lack) {
          break;
        }
      }

      // unpack不断循环直到解压数据被读取完
      fileUnpacker.unpackFilesByBlock(unzippedData, false);

      buffer.resize(PACK_BLOCK_SIZE);  // 重置缓冲区大小
      unzippedData.clear();
      decryptedData.clear();

      if (flush) {
        // 没有数据了，退出
        break;
      }
    }

    // 最后一次flush
    fileUnpacker.unpackFilesByBlock(unzippedData, true);
  } catch (std::exception& e) {
    throw std::runtime_error(
      "Error occurred when trying to unpack file, its uuid is " + uuid +
      ", because " + std::string(e.what())
    );
  }
}

/**
 * @brief 给定uuid，读取指定的目录文件
 *
 */
Json::Value readDirectoryFileById(const std::string& uuid) {
  CommitTableRecord cr = CommitTable::getCommitRecordViewById(uuid);

  fs::path path = cr.storagePath + "/" + uuid + "/directoryfile";

  return readDirectoryFile(path);
}

/**
 * @brief 读取指定的目录文件，还原成目录树(json形式)
 *
 */
Json::Value readDirectoryFile(const fs::path& src) {
  std::ifstream inFile(src);
  if (!inFile) {
    throw std::runtime_error("Failed to open file: " + src.string());
  }

  Json::Value root;
  inFile >> root;

  inFile.close();
  return root;
}

}  // namespace zipfiles::server