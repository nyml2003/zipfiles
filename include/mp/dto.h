#ifndef ZIPFILES_MP_DATA_TRANSFER_OBJECT_H
#define ZIPFILES_MP_DATA_TRANSFER_OBJECT_H

#include <sys/types.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles {
/**
 * @brief Filter的DTO
 *
 */
struct FilterDTO {
  std::optional<fs::file_type> type;
  std::optional<__off_t> minSize;
  std::optional<__off_t> maxSize;
  std::optional<double> minCreateTime;
  std::optional<double> maxCreateTime;
  std::optional<double> minUpdateTime;
  std::optional<double> maxUpdateTime;
  std::optional<std::string> owner;
  std::optional<std::string> group;
};

/**
 * @brief FileDetail的DTO
 *
 */
struct FileDetailDTO {
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
 * @brief Commit的DTO
 *
 */
struct CommitDTO {
  // 包含的文件数组
  std::vector<fs::path> files;
  // id为Commit的唯一标识
  std::string uuid;
  // Commit消息
  std::string message;
  // 创建时间
  // 以秒为单位，有效位数是53位
  double createTime;
  // 文件的存储路径
  std::string storagePath;
  // 作者
  std::string author;
  // 是否被加密
  bool isEncrypt;
  // 加密密钥
  std::optional<std::string> key;
};

}  // namespace zipfiles
   //
#endif  // !ZIPFILES_MP_COMMIT_Hs