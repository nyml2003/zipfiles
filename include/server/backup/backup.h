#ifndef ZIPFILES_SERVER_BACKUP_H
#define ZIPFILES_SERVER_BACKUP_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include "server/tools/committable.h"

namespace fs = std::filesystem;

namespace zipfiles::server {
/**
 * * backup
 *
 */
void backupFiles(
  const std::vector<fs::path>& files,
  CommitTableRecord& cr,
  const std::string& key
);

fs::path getCommonAncestor(const std::vector<fs::path>& paths);

void removeCommitById(const std::string& uuid);

void removeCommitAndRecordById(const std::string& uuid);

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