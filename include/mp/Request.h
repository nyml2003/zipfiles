#ifndef ZIPFILES_MP_REQUEST_H
#define ZIPFILES_MP_REQUEST_H
#include <json/json.h>
#include <optional>
#include <string>
#include <variant>
#include "json/value.h"
#include "mp/dto.h"
namespace zipfiles {
namespace request {

struct GetFileDetail {
  std::string path;
};

struct GetFileList {
  std::string path;
  std::optional<Filter> filter;
};

struct GetAllFileDetails {
  std::string path;
};

// 模拟耗时请求
struct MockNeedTime {
  int id;
};

struct PostCommit {
  CommitLog commitLog;
};

};  // namespace request

struct Req;

using ReqKind = std::variant<
  request::GetFileDetail,
  request::GetFileList,
  request::MockNeedTime,
  request::PostCommit,
  request::GetAllFileDetails>;

using ReqPtr = std::shared_ptr<Req>;

struct Req {
  explicit Req(ReqKind kind);
  ReqKind kind;
  double timestamp{};
  std::string uuid;
  Json::Value toJson();
  static ReqPtr fromJson(const Json::Value& json);
};

ReqPtr makeReqGetFileDetail(std::string path);
ReqPtr makeReqGetFileDetail(Json::Value payload);
ReqPtr makeReqGetFileList(std::string path);
ReqPtr makeReqGetFileList(std::string path, Filter filter);
ReqPtr makeReqGetFileList(Json::Value payload);
ReqPtr makeReqMockNeedTime(int id);
ReqPtr makeReqMockNeedTime(Json::Value payload);
ReqPtr makeReqPostCommit(CommitLog commitLog);
ReqPtr makeReqPostCommit(Json::Value payload);
ReqPtr makeReqGetAllFileDetails(std::string path);
ReqPtr makeReqGetAllFileDetails(Json::Value payload);

}  // namespace zipfiles

#endif