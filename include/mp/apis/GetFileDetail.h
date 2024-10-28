#ifndef ZIPFILES_MP_APIS_GET_FILEDETAIL_H
#define ZIPFILES_MP_APIS_GET_FILEDETAIL_H

#include <json/json.h>
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

struct GetFileDetail : public getFileDetail::FileDetail {};
}  // namespace response

ReqPtr makeReqGetFileDetail(std::string path, std::string name);

ReqPtr makeReqGetFileDetail(Json::Value payload);

ResPtr makeResGetFileDetail(response::getFileDetail::FileDetail file);

ResPtr makeResGetFileDetail(Json::Value payload);

}  // namespace zipfiles

#endif