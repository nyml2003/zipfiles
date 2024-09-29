#ifndef ZIPFILES_MP_REQUEST_H
#define ZIPFILES_MP_REQUEST_H
#include <json/json.h>
#include <string>
#include <variant>
namespace zipfiles {
namespace request {

struct GetFileDetail {
  std::string path;
};

struct GetFileList {
  std::string path;
};

};  // namespace request

struct Req;

using ReqKind = std::variant<request::GetFileDetail, request::GetFileList>;
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
ReqPtr makeReqGetFileList(std::string path);

}  // namespace zipfiles

#endif