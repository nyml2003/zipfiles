#ifndef ZIPFILES_SERVER_TOOLS_FSAPI_H
#define ZIPFILES_SERVER_TOOLS_FSAPI_H

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles::server {
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
  // 绝对路径
  std::string absolutePath;
  // 设备号
  dev_t dev{};
};

/**
 * @brief 给定目录和过滤器，返回经过过滤的目录下的所有文件
 *
 */
// std::vector<File>
// getFileList(const fs::path& directory, bool doFilter, MetaDataFilter&
// filter);

/**
 * @brief 给定路径，返回指定路径的FileDetail
 *
 */
FileDetail getFileDetail(const fs::path& file);

/**
 * @brief 获取指定目录下的所有FileDetail
 *
 */
std::vector<FileDetail> getAllFileDetails(const fs::path& directory);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_FSAPI_H