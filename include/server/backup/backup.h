#ifndef ZIPFILES_SERVER_BACKUP_H
#define ZIPFILES_SERVER_BACKUP_H

#include <filesystem>
#include <vector>
#include "mp/dto.h"

namespace zipfiles::server {

namespace fs = std::filesystem;

void backupFiles(const std::vector<fs::path>& files, const CommitLog& cl);

DirectoryTreeNode generateDirectoryTree(
  const std::vector<const fs::path&>& files
);

bool isCommitted(const CommitLog& cl);

void writeCommitLog(const fs::path& dst, const CommitLog& cl);

void writeDirectoryFile(const fs::path& dst, const DirectoryTreeNode& root);

void deleteCommit();

void deleteCommitLog();

}  // namespace zipfiles::server
#endif  // !ZIPFILES_BACKUP_H