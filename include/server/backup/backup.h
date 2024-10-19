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

fs::path getCommonAncestor(const std::vector<fs::path>& paths);

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