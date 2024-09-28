#ifndef ZIPFILES_SERVER_RESTORE_H
#define ZIPFILES_SERVER_RESTORE_H
#include <filesystem>
#include "common.h"

namespace zipfiles::server {
DirectoryTreeNode readDirectoryFile(const fs::path& src);
std::vector<CommitLog> readCommitLog(const fs::path& src);
}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_RESTORE_H