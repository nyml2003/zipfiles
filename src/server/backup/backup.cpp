#include <filesystem>
#include <stdexcept>
#include <vector>
#include "server/backup/backup.h"

namespace zipfiles::server {
/**
 * @brief 前端的直接调用接口，处理整个commit流程
 *
 * @param files 一个文件路径数组
 */
void backupFiles(
  const std::vector<const fs::path&>& /*files*/,
  const CommitLog& cl
) {
  if (isCommitted(cl)) {
    throw std::runtime_error("Commit log is already committed.");
  }
}

/**
 * @brief 依据传进的路径数组生成一个目录树
 *
 */
DirectoryTreeNode generateDirectoryTree(
  const std::vector<const fs::path&>& /*files*/
) {
  DirectoryTreeNode root;
  return root;
}

/**
 * @brief 给定log文件的路径，在末尾添加一个CommitLog
 *
 */
void writeCommitLog(const fs::path& /*dst*/, const CommitLog& /*cl*/) {}

/**
 * @brief 给定一个路径，生成一个描述Commit内容的目录树文件
 *
 */
void writeDirectoryFile(
  const fs::path& /*dst*/,
  const DirectoryTreeNode& /*root*/
) {}

/**
 * @brief 判断一个CommitLog是否已经被提交
 *
 */
bool isCommitted(const CommitLog& /*cl*/) {
  return false;
}

}  // namespace zipfiles::server