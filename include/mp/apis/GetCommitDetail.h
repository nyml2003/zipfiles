#ifndef ZIPFILES_MP_APIS_GET_COMMIT_DETAIL_H
#define ZIPFILES_MP_APIS_GET_COMMIT_DETAIL_H

#include <json/json.h>
#include <sys/types.h>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles {
struct Req;
using ReqPtr = std::shared_ptr<Req>;
struct Res;
using ResPtr = std::shared_ptr<Res>;

namespace request {
struct GetCommitDetail {
  std::string uuid;
};
}  // namespace request

namespace response {

namespace getCommitDetail {
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
}  // namespace getCommitDetail

struct GetCommitDetail {
  std::vector<getCommitDetail::FileDetail> files;
};
}  // namespace response

ReqPtr makeReqGetCommitDetail(std::string uuid);
ReqPtr makeReqGetCommitDetail(Json::Value payload);
ResPtr makeResGetCommitDetail(
  std::vector<response::getCommitDetail::FileDetail> files
);
ResPtr makeResGetCommitDetail(Json::Value payload);
}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_GET_COMMIT_DETAIL_H