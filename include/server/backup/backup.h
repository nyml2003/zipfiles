#ifndef ZIPFILES_SERVER_BACKUP_H
#define ZIPFILES_SERVER_BACKUP_H

#include <filesystem>
#include <vector>
#include "json/value.h"
#include "mp/dto.h"

namespace zipfiles::server {

namespace fs = std::filesystem;

void backupFiles(const std::vector<fs::path>& files, const Json::Value& cl);

bool isCommitted(const Json::Value& cls, const Json::Value& cl);

void appendCommitLog(Json::Value& dst, const Json::Value& cl);

void writeCommitLog(const fs::path& dst, const Json::Value& cls);

void writeDirectoryFile(const fs::path& dst, const DirectoryTreeNode& root);

DirectoryTreeNode generateDirectoryTree(
  const std::vector<const fs::path&>& files
);

void deleteCommit();

void deleteCommitLog();

}  // namespace zipfiles::server
#endif  // !ZIPFILES_BACKUP_H