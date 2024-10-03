#ifndef ZIPFILES_SERVER_BACKUP_H
#define ZIPFILES_SERVER_BACKUP_H

#include <cstdint>
#include <filesystem>
#include <vector>
#include "json/value.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

/**
 * * backup
 *
 */
void backupFiles(
  const std::vector<fs::path>& files,
  const Json::Value& cl,
  const std::string& key
);

std::vector<uint8_t> readFile(const fs::path& filepath);

void writeFile(const fs::path& filepath, const std::vector<uint8_t>& data);

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
  const std::vector<fs::path>& files
);

}  // namespace zipfiles::server
#endif  // !ZIPFILES_BACKUP_H