#ifndef ZIPFILES_COMMITTABLE_H
#define ZIPFILES_COMMITTABLE_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include "json/value.h"

namespace fs = std::filesystem;

namespace zipfiles::server {
/**
 * @brief commit结构
 *
 */
struct CommitTableRecord {
  // id为Commit的唯一标识
  std::string uuid;
  // Commit消息
  std::string message;
  // 创建时间
  // 以秒为单位，有效位数是53位
  double createTime;
  // 文件的存储路径
  std::string storagePath;
  // 作者
  std::string author;
  // 是否被加密
  bool isEncrypt;
  // 是否被删除
  bool isDelete;
};

/**
 * @brief CommitTable类，单例
 *
 */
class CommitTable {
 public:
  static CommitTable& getInstance() {
    static CommitTable instance;
    return instance;
  }

  static void readCommitTable(const fs::path& src);

  static bool isCommitted(const Json::Value& cr);

  static void writeCommitTable(const fs::path& dst);

  static void deleteCommitRecord(const std::string& uuid);

  static void removeCommitRecord(const std::string& uuid);

  static Json::Value getCommitRecordById(const std::string& uuid);

  CommitTable(const CommitTable& other) = delete;
  CommitTable& operator=(const CommitTable& other) = delete;
  CommitTable(CommitTable&& other) noexcept = delete;
  CommitTable& operator=(CommitTable&& other) noexcept = delete;

 private:
  CommitTable();
  ~CommitTable();
  Json::Value commitTable;
  std::mutex mutex;

  static void appendCommitRecord(const Json::Value& cr);
};

}  // namespace zipfiles::server

#endif