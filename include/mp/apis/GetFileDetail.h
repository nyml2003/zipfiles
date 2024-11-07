#ifndef ZIPFILES_MP_APIS_GET_FILE_DETAIL_H
#define ZIPFILES_MP_APIS_GET_FILE_DETAIL_H

#include <sys/types.h>
#include <filesystem>

namespace fs = std::filesystem;
namespace zipfiles {
struct Req;
using ReqPtr = std::shared_ptr<Req>;
struct Res;
using ResPtr = std::shared_ptr<Res>;

namespace request {

struct GetFileDetail {
  std::string path;
  std::string name;
};
}  // namespace request

namespace response {

namespace getFileDetail {
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
}  // namespace getFileDetail

using GetFileDetail = getFileDetail::FileDetail;

}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_GET_FILE_DETAIL_H