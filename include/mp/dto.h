#ifndef ZIPFILES_MP_DATA_TRANSFER_OBJECT_H
#define ZIPFILES_MP_DATA_TRANSFER_OBJECT_H
#include <filesystem>
#include <string>
#include <vector>
namespace zipfiles {
namespace fs = std::filesystem;
/**
 * @brief 用以展示目录结构的结构体
 *
 */
struct DirectoryTreeNode {
  // 文件名
  std::string name;
  // 文件类型，定义见filesystem
  fs::file_type type;
  // 子节点
  std::vector<DirectoryTreeNode> children;
};

/**
 * @brief 记录commit的日志结构
 *
 */
struct CommitLog {
  // Commit消息，默认为Commit的唯一标识
  std::string message;
  // 创建时间
  std::string createTime;
  // 恢复的默认路径
  std::string defaultPath;
  // 作者
  std::string author;
};
/**
 * @brief 选中文件后展示的文件信息
 *
 */
struct FileDetail {
  // 文件类型，定义见filesystem
  fs::file_type type{};
  // 创建时间
  double createTime;
  // 修改时间
  double updateTime;
  // 大小
  __off_t size{};
  // 创建用户
  std::string owner;
  // 用户属于的用户组
  std::string group;
  // 权限
  mode_t mode{};
  // 路径
  std::string path;
  // 文件名
  std::string name;
};
struct File {
  // 文件名
  std::string name;
  // 文件类型，定义见filesystem
  fs::file_type type;
};
}  // namespace zipfiles
#endif  // !ZIPFILES_MP_COMMIT_Hs