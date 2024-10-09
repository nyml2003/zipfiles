#ifndef ZIPFILES_MP_DATA_TRANSFER_OBJECT_H
#define ZIPFILES_MP_DATA_TRANSFER_OBJECT_H

#include <sys/types.h>
#include <filesystem>
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
  // 文件名
  std::string name;
  // 设备号
  dev_t dev{};
};

/**
 * @brief 记录commit的日志结构
 *
 */
struct CommitLog {
  // id为Commit的唯一标识
  std::string uuid;
  // Commit消息
  std::string message;
  // 创建时间
  // 以秒为单位，有效位数是53位
  double createTime;
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