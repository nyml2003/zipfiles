#include <filesystem>
#include <stdexcept>
#include <vector>
#include "mp/dto.h"
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
 * @param files 一个文件路径数组
 */
DirectoryTreeNode generateDirectoryTree(const std::vector<fs::path>& files) {
  // 插入根目录
  DirectoryTreeNode root;
  root.name = "/";
  root.type = fs::file_type::directory;

  for (const auto& file : files) {
    DirectoryTreeNode* cur = &root;

    for (const auto& part : file) {
      std::string partStr = part.string();

      if (cur->children.find(partStr) == cur->children.end()) {
        // 没有找到已经记录的子节点
        DirectoryTreeNode newNode;
        newNode.name = partStr;
        newNode.type = fs::symlink_status(file).type();
        cur->children[partStr] = newNode;
      }

      // 这个节点已经记录了一个目录信息，继续深入
      cur = &cur->children[partStr];
    }
  }

  return root;
}

/**
 * @brief 给定log文件的路径，在末尾添加一个CommitLog
 *
 * @param dst 指定的log文件路径
 *
 * @param cl 指定的CommitLog对象
 */
void writeCommitLog(const fs::path& /*dst*/, const CommitLog& /*cl*/) {}

/**
 * @brief 给定一个路径，生成一个描述Commit内容的目录树文件
 *
 * @param dst 指定的目录文件路径
 *
 * @param root 用以创建目录文件的目录树根目录
 */
void writeDirectoryFile(
  const fs::path& /*dst*/,
  const DirectoryTreeNode& /*root*/
) {}

/**
 * @brief 判断一个CommitLog是否已经被提交
 *
 * @param src 指定的log文件路径
 *
 * @param cl 指定的CommitLog对象
 */
bool isCommitted(const CommitLog& /*cl*/) {
  return false;
}

}  // namespace zipfiles::server