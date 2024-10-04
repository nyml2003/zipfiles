#include "server/backup/backup.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <log4cpp/Category.hh>
#include <stdexcept>
#include <vector>
#include "json/value.h"
#include "json/writer.h"
#include "mp/dto.h"
#include "server/crypto/crypto.h"
#include "server/deflate/zip.h"
#include "server/pack/pack.h"
#include "server/restore/restore.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {

/**
 * @brief 前端的直接调用接口，处理整个commit流程
 *
 * @param files 一个文件路径数组，要求是绝对路径
 *
 * @param cl 一个commitlog的json对象
 *
 * @param key 加密密钥
 */
void backupFiles(
  const std::vector<fs::path>& files,
  const Json::Value& cl,
  const std::string& key
) {
  log4cpp::Category::getRoot().infoStream()
    << "Backup started, log messeag is \"" << cl["message"].asString()
    << "\" at " << cl["createTime"].asString();

  // log文件地址
  // ? 待更改
  fs::path src = "~/.zip/commit.log";

  // 读出后保存当前视图
  Json::Value cls = readCommitLog(src);

  // 检查是否提交过
  if (isCommitted(cls, cl)) {
    throw std::runtime_error(
      "Commit log is already committed, its uuid is " + cl["uuid"].asString()
    );
  }

  // 创建输出目录
  std::string path = cl["storagePath"].asString();
  fs::path dir = fs::path(path).parent_path();

  if (!fs::exists(dir)) {
    fs::create_directories(dir);
  }

  // 打开输出流
  std::ofstream outputFile(path, std::ios::binary);
  if (!outputFile) {
    throw std::runtime_error("Failed to open: " + path);
  }

  // 获取是否加密
  bool encrypt = cl["isEncrypt"].asBool();

  // 实例化加密IV
  std::array<CryptoPP::byte, AES::BLOCKSIZE> iv{};

  // 实例化加密类
  AESEncryptor encryptor(key);

  if (encrypt) {
    // 如果需要加密
    AutoSeededRandomPool prng;
    prng.GenerateBlock(iv.data(), iv.size());

    // 把IV写入文件开头，这部分不需要压缩和加密
    outputFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());
  }

  bool flush = false;

  // 主循环
  try {
    while (true) {
      std::vector<uint8_t> processedData{};

      // 获取输出
      auto [packFlush, packedData] = packFilesByBlock(files, flush);

      // 遍历pack的obuffer
      for (unsigned char byte : packedData) {
        // 将pack的obuffer的每个字节都加入zip的ibuffer
        auto [zipFlush, zippedData] = zip(byte, flush);

        if (zipFlush) {
          // 如果zip的ibuffer满，那么压缩，并输出到processedData
          processedData.insert(
            processedData.end(), zippedData.begin(), zippedData.end()
          );
          // 清空zip的obuffer
          zippedData.clear();  // 清空缓冲区
        }
      }

      // 当processedData不为空
      if (!processedData.empty()) {
        // 如果需要加密，此时则将压缩后的processedData加密
        if (encrypt) {
          processedData = encryptor.encryptFile(processedData, iv);
        }

        // 写入输出流
        outputFile.write(
          reinterpret_cast<const char*>(processedData.data()),
          static_cast<std::streamsize>(processedData.size())
        );
      }

      // 如果所有文件都读取完毕，并且pack的obuffer还有内容，设置 flush 为 true
      // 下一次循环会将pack的obuffer强制输出，加入zip的ibuffer，并且zip也会强制输出obuffer
      // (pack的obuffer是zip的ibuffer的二分之一，因此不需要担心溢出，就算溢出也会全部写入processedData)
      // 强制输出后，packFlush为true，flush也是true，此时可以退出
      if (!packFlush && !packedData.empty()) {
        flush = true;
      }

      // 退出循环条件
      if (packFlush && flush) {
        break;
      }
    }

  } catch (std::exception& e) {
    // 移除失败文件
    fs::remove(path);
    outputFile.close();

    throw std::runtime_error(
      "Error occurred when trying to pack files, its uuid is " +
      cl["uuid"].asString() + ", because " + std::string(e.what())
    );
  }

  // 生成目录文件
  try {
    // todo: 目录文件的dst还没有确定
    writeDirectoryFile(
      fs::path(cl["storagePath"].asString()).parent_path() / "directoryfile",
      files
    );
  } catch (std::exception& e) {
    throw std::runtime_error(
      "Error occurred when trying to write directory file, its uuid is " +
      cl["uuid"].asString() + ", because " + std::string(e.what())
    );
  }

  // 完成后添加到commitlog
  try {
    appendCommitLog(cls, cl);
    writeCommitLog(src, cls);
  } catch (const std::exception& e) {
    throw std::runtime_error(
      "Error occurred when trying to append commit log, its uuid is " +
      cl["uuid"].asString() + ", because " + std::string(e.what())
    );
  }
}

/**
 * @brief 读取文件为一个字节流
 *
 * @param filepath 文件的路径
 *
 * ! Deprecated
 *
 */
std::vector<uint8_t> readFile(const fs::path& filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filepath.string());
  }

  std::vector<uint8_t> data(
    (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()
  );

  if (file.fail() && !file.eof()) {
    throw std::runtime_error("Failed to read file: " + filepath.string());
  }

  return data;
}

/**
 * @brief 把字节流写入文件
 *
 * @param filepath 目的路径
 *
 * @param data 字节流
 *
 * ! Deprecated
 *
 */
void writeFile(const fs::path& filepath, const std::vector<uint8_t>& data) {
  std::ofstream file(filepath, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filepath.string());
  }

  file.write(
    reinterpret_cast<const char*>(data.data()),
    static_cast<std::streamsize>(data.size())
  );

  if (file.fail()) {
    throw std::runtime_error("Failed to write to file: " + filepath.string());
  }
}

/**
 * @brief 获取一个绝对路径数组的最近公共父目录
 *
 * @param paths 绝对路径数组
 *
 * ? 是否可以用字典树优化
 */

fs::path getCommonAncestor(const std::vector<fs::path>& paths) {
  if (paths.empty()) {
    throw std::runtime_error("Paths array is empty");
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
 * @brief 判断一个CommitLog是否已经被提交
 *
 * @param src 指定的CommitLog文件，以json的形式展示
 *
 * @param cl 指定的CommitLog对象
 *
 */
bool isCommitted(const Json::Value& cls, const Json::Value& cl) {
  if (!(cls.isMember("data") && cls["data"].isArray())) {
    throw std::runtime_error("Illegal Json format");
  }

  return std::any_of(
    cls["data"].begin(), cls["data"].end(),
    [&cl](const Json::Value& log) {
      return log.isMember("uuid") &&
             log["uuid"].asString() == cl["uuid"].asString();
    }
  );
}

/**
 * @brief 给定已有的commitlog文件(json形式)，在末尾添加一个CommitLog
 *
 * @param dst 指定的log文件路径
 *
 * @param cl 指定的CommitLog对象
 *
 */
void appendCommitLog(Json::Value& dst, const Json::Value& cl) {
  if (dst.isMember("data") && dst["data"].isArray()) {
    dst["data"].append(cl);
  } else {
    throw std::runtime_error("Illegal Json format");
  }
}

/**
 * @brief 给定一个路径，将commitLogs文件写入(json形式)
 *
 * @param dst 指定的log文件路径
 *
 * @param cls 指定的CommitLogs对象
 *
 */
void writeCommitLog(const fs::path& dst, const Json::Value& cls) {
  // 写回文件
  std::ofstream logFileWrite(dst, std::ios::binary);
  logFileWrite << cls.toStyledString();
  logFileWrite.close();
}

/**
 * @brief 给定一个路径，生成一个描述Commit内容的目录树文件(json形式)
 *
 * @param dst 指定的目录文件路径
 *
 * @param root 用以创建目录文件路径
 *
 */
void writeDirectoryFile(
  const fs::path& dst,
  const std::vector<fs::path>& files
) {
  std::ofstream outFile(dst);
  if (!outFile) {
    throw std::runtime_error("Failed to open file: " + dst.string());
  }

  Json::Value root;
  root["data"] = Json::arrayValue;

  for (const auto& path : files) {
    FileDetail fd = getFileDetail(path);
    Json::Value temp;

    temp["type"] = static_cast<int>(fd.type);  // 将文件类型转换为整数
    temp["createTime"] = fd.createTime;
    temp["updateTime"] = fd.updateTime;
    temp["size"] = static_cast<Json::Int64>(fd.size);  // 确保大小是64位整数
    temp["owner"] = fd.owner;
    temp["group"] = fd.group;
    temp["mode"] = fd.mode;
    temp["path"] = fd.path;
    temp["name"] = fd.name;

    root["data"].append(temp);
  }

  Json::StreamWriterBuilder writer;
  outFile << Json::writeString(writer, root);
  outFile.close();
}

}  // namespace zipfiles::server
