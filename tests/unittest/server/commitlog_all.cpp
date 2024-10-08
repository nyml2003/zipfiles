#include <json/json.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "server/backup/backup.h"
#include "server/restore/restore.h"

namespace fs = std::filesystem;

using namespace zipfiles::server;

void displayFileContent(const fs::path& filePath) {
  std::ifstream file(filePath);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      std::cout << line << std::endl;
    }
    file.close();
  } else {
    std::cerr << "Unable to open file: " << filePath << std::endl;
  }
}

void processCommitLog(
  const fs::path& logFilePath,
  const Json::Value& newCommitLog
) {
  try {
    // 读取现有的commit log文件
    Json::Value commitLogs = readCommitLog(logFilePath);

    // 输出读取到的commit log内容
    std::cout << "Commit logs before update:" << std::endl;
    displayFileContent(logFilePath);

    // 检查新提交的commit log是否已经存在
    if (!isCommitted(commitLogs, newCommitLog)) {
      // 如果不存在，则添加新的commit log
      appendCommitLog(commitLogs, newCommitLog);

      // 将更新后的commit log写回文件
      writeCommitLog(logFilePath, commitLogs);

      // 输出更新后的commit log内容
      std::cout << "Commit logs after update:" << std::endl;
      displayFileContent(logFilePath);

      std::cout << "New commit log added successfully." << std::endl;
    } else {
      std::cout << "Commit log already exists." << std::endl;
    }

    // 清理：删除commit log文件
    fs::remove(logFilePath);
    std::cout << "Commit log file cleaned up." << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

int main() {
  // 定义commit log文件路径
  fs::path logFilePath = "commit_log.json";

  // 创建一个新的commit log对象
  Json::Value newCommitLog;
  newCommitLog["uuid"] = "5678";
  newCommitLog["message"] = "Second commit";

  // 处理commit log
  processCommitLog(logFilePath, newCommitLog);

  return 0;
}