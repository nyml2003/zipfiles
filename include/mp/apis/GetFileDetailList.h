#ifndef ZIPFILES_MP_APIS_GET_FILEDETAIL_LIST_H
#define ZIPFILES_MP_APIS_GET_FILEDETAIL_LIST_H

#include <sys/types.h>
#include <filesystem>
#include <optional>
#include <vector>

namespace fs = std::filesystem;
namespace zipfiles {
struct Req;
using ReqPtr = std::shared_ptr<Req>;
struct Res;
using ResPtr = std::shared_ptr<Res>;

namespace request {

namespace getFileDetailList {
struct Filter {
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
}  // namespace getFileDetailList

struct GetFileDetailList {
  std::string path;
  std::optional<getFileDetailList::Filter> filter;
};
}  // namespace request

namespace response {

namespace getFileDetailList {
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
}  // namespace getFileDetailList

struct GetFileDetailList {
  std::vector<getFileDetailList::FileDetail> files;
};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_GET_FILE_LIST_H