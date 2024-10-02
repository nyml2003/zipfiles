#include <exception>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "json/reader.h"
#include "json/value.h"
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
 */
void restoreTo(const fs::path& dst, const std::string& uuid) {
  // log文件地址
  // ? 待更改
  fs::path src = "~/.zip/commit.log";

  Json::Value cls = readCommitLog(src);

  Json::Value cl = getCommitLogById(cls, uuid);

  // 创建指定的目录
  fs::create_directories(dst);

  // todo: 解密
  // todo: 解压缩

  // 解包
  try {
    // ?
    // 如果解密和解压已经在目标目录产生了一个临时文件，那么src参数就是那个临时文件的路径
    // 但是这里还没有确定具体逻辑
    unpackArchive(fs::path(cl["storagePath"].asString()), dst);
  } catch (std::exception& e) {
    throw std::runtime_error(
      "Error occurred when trying to unpack " + std::string(e.what())
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