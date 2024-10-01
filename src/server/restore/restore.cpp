
#include <fstream>
#include "json/reader.h"
#include "json/value.h"
#include "server/restore/restore.h"

namespace zipfiles::server {

/**
 * @brief 读取指定的目录文件，还原成目录树
 *
 */
DirectoryTreeNode readDirectoryFile(const fs::path& /*src*/) {
  DirectoryTreeNode root;
  return root;
}

/**
 * @brief 读取指定的CommitLog文件，返回一个Json::Value数组
 *
 * @param src 指定的log文件路径
 *
 * @return Json::Value 包含所有CommitLog的Json数组
 */
Json::Value readCommitLog(const fs::path& src) {
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

}  // namespace zipfiles::server