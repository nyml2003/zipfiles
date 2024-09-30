
#include <fstream>
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

CommitLog parseCommitLog(const std::string& logEntry) {
  std::istringstream stream(logEntry);
  CommitLog cl;
  std::string line;

  Json::Value json;

  while (std::getline(stream, line)) {
    if (line.find("UUID: ") == 0) {
      cl.uuid = line.substr(6);
    } else if (line.find("Message: ") == 0) {
      cl.message = line.substr(9);
    } else if (line.find("CreateTime: ") == 0) {
      cl.createTime = std::stod(line.substr(12));
    } else if (line.find("DefaultPath: ") == 0) {
      cl.defaultPath = line.substr(13);
    } else if (line.find("StoragePath: ") == 0) {
      cl.storagePath = line.substr(13);
    } else if (line.find("Author: ") == 0) {
      cl.author = line.substr(8);
    } else if (line.find("IsEncrypt: ") == 0) {
      cl.isEncrypt = (line.substr(11) == "true");
    } else if (line.find("IsDelete: ") == 0) {
      cl.isDelete = (line.substr(10) == "true");
    }
  }

  return cl;
}

/**
 * @brief 读取指定的CommitLog文件，返回一个CommitLog数组
 *
 * todo: 将CommitLog替换为Json value形式
 */
std::vector<CommitLog> readCommitLog(const fs::path& src) {
  std::ifstream logFile(src);
  if (!logFile.is_open()) {
    throw std::runtime_error("Unable to open log file");
  }

  std::vector<CommitLog> commitLogs;
  std::string line;
  std::string logEntry;

  while (std::getline(logFile, line)) {
    if (line == "-") {
      commitLogs.push_back(parseCommitLog(logEntry));
      logEntry.clear();
    } else {
      logEntry += line + "\n";
    }
  }

  if (!logEntry.empty()) {
    commitLogs.push_back(parseCommitLog(logEntry));
  }

  logFile.close();
  return commitLogs;
}

}  // namespace zipfiles::server