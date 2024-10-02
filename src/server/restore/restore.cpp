
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

/**
 * @brief 读取指定的CommitLog文件，返回一个CommitLog数组
 *
 */
std::vector<CommitLog> readCommitLog(const fs::path& /*src*/) {
  std::vector<CommitLog> commitLogs;
  return commitLogs;
}

}  // namespace zipfiles::server