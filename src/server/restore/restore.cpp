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
#include "server/backup/backup.h"
#include "server/crypto/crypto.h"
#include "server/deflate/zip.h"
#include "server/pack/unpack.h"
#include "server/restore/restore.h"

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

  // log文件地址
  // ? 待更改
  fs::path src = std::getenv("HOME") + std::string("/.zip/commit.log");

  Json::Value cls = readCommitLog(src);

  Json::Value cl = getCommitLogById(cls, uuid);

  // 检查目标路径是否存在，如果不存在则创建目录
  if (!fs::exists(dst)) {
    fs::create_directories(dst);
  }

  // 打开输入流
  std::string filePath = cl["storagePath"].asString() + "/" +
                         cl["uuid"].asString() + "/" + cl["uuid"].asString();
  std::ifstream inFile(filePath, std::ios::binary);
  if (!inFile) {
    throw std::runtime_error("Failed to open: " + filePath);
  }

  // 如果有加密，则读取IV
  bool decrypt = cl["isEncrypt"].asBool();
  std::array<CryptoPP::byte, AES::BLOCKSIZE> iv{};
  if (decrypt) {
    inFile.read(reinterpret_cast<char*>(iv.data()), iv.size());
  }

  // 实例化解码器
  AESEncryptor decryptor(key);

  // 实例化解包器
  FileUnpacker fileUnpacker(dst);

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
      for (auto byte : decryptedData) {
        auto [done, outputData] = unzip(byte);
        if (done) {
          unzippedData.insert(
            unzippedData.end(), outputData.begin(), outputData.end()
          );
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
 * @brief 读取指定的CommitLog文件，返回一个Json::Value数组
 *
 * @param src 指定的log文件绝对路径
 *
 * @return 包含所有CommitLog的Json数组
 *
 */
Json::Value readCommitLog(const fs::path& src) {
  // 先创建相应的目录
  // src有可能是一个相对目录或者根目录，此时是没有parent_path的
  // 要求src是一个绝对路径
  if (src.has_parent_path()) {
    fs::create_directories(src.parent_path());
  }

  // 先打开文件输出流，因为可能目标不存在，那么此时会创造一个新文件
  std::ofstream logFileNew(src, std::ios::app);
  logFileNew.close();

  // 再以输入流打开文件
  std::ifstream logFile(src);
  if (!logFile.is_open()) {
    throw std::runtime_error("Unable to open log file");
  }

  // 把文件指针移到末尾
  logFile.seekg(0, std::ios::end);

  if (logFile.tellg() == 0) {
    // 空文件错误
    logFile.close();

    // 创建一个新的Json对象
    Json::Value root;
    root["data"] = Json::arrayValue;

    std::ofstream logFileWrite(src);
    logFileWrite << root.toStyledString();
    logFileWrite.close();

    // 重新打开文件
    logFile.open(src);
    if (!logFile.is_open()) {
      throw std::runtime_error("Unable to open log file");
    }
  } else {
    // 不是空，那么回移指针
    logFile.seekg(0, std::ios::beg);
  }

  Json::Value commitLogs;
  Json::CharReaderBuilder readerBuilder;
  std::string errs;

  if (!Json::parseFromStream(readerBuilder, logFile, &commitLogs, &errs)) {
    // 若出错，说明文件是错误编码的
    throw std::runtime_error("Failed to parse JSON: " + errs);
  }

  logFile.close();

  return commitLogs;
}

/**
 * @brief 给定uuid，返回指定的commitlog
 *
 */
Json::Value getCommitLogById(const Json::Value& cls, const std::string& uuid) {
  if (!(cls.isMember("data") && cls["data"].isArray())) {
    throw std::runtime_error("Illegal Json format");
  }

  for (const auto& log : cls["data"]) {
    // 检查每个元素是否包含"uuid"字段且和目标uuid匹配
    if (log.isMember("uuid") && log["uuid"].asString() == uuid) {
      return log;
    }
  }

  // 找不到对应的commit log
  throw std::runtime_error(
    "Cannot find specific commit log by given uuid " + uuid
  );
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