#ifndef ZIPFILES_MP_DATA_TRANSFER_OBJECT_H
#define ZIPFILES_MP_DATA_TRANSFER_OBJECT_H

#include <filesystem>
#include <map>
#include <string>

namespace zipfiles {
namespace fs = std::filesystem;

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

/**
 * @brief 用以展示目录结构的结构体
 *
 */
struct DirectoryTreeNode : public FileDetail {
  std::map<std::string, DirectoryTreeNode> children;
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
  // 文件的存储路径
  std::string storagePath;
  // 作者
  std::string author;
  // 是否被加密
  bool isEncrypt;
  // 是否被删除
  bool isDelete;
};

struct File {
  // 文件名
  std::string name;
  // 文件类型，定义见filesystem
  fs::file_type type;
};

}  // namespace zipfiles
   //
#endif  // !ZIPFILES_MP_COMMIT_Hs