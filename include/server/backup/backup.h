#ifndef ZIPFILES_SERVER_BACKUP_H
#define ZIPFILES_SERVER_BACKUP_H

#include <cstdint>
#include <filesystem>
#include <vector>
#include "json/value.h"

namespace fs = std::filesystem;

namespace zipfiles::server {
/**
 * @brief 记录commit的日志结构
 *
 */
struct CommitLog {
  // id为Commit的唯一标识
  std::string uuid;
  // Commit消息
  std::string message;
  // 创建时间
  // 以秒为单位，有效位数是53位
  double createTime;
  // 恢复的默认路径
  std::string defaultPath;
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
 * * backup
 *
 */
void backupFiles(
  const std::vector<fs::path>& files,
  const Json::Value& cl,
  const std::string& key
);

fs::path getCommonAncestor(const std::vector<fs::path>& paths);

/**
 * * commit log
 *
 */
bool isCommitted(const Json::Value& cls, const Json::Value& cl);

void appendCommitLog(Json::Value& dst, const Json::Value& cl);

void writeCommitLog(const fs::path& dst, const Json::Value& cls);

void deleteCommitLog(Json::Value& cls, const std::string& uuid);

/**
 * * directory
 *
 */
void writeDirectoryFile(
  const fs::path& dst,
  const std::vector<fs::path>& files,
  const fs::path& lca
);

}  // namespace zipfiles::server
#endif  // !ZIPFILES_BACKUP_H