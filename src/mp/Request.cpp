#include "mp/Request.h"

#include <utility>
#include "common.h"

namespace zipfiles {
Req::Req(ReqKind kind) : kind(std::move(kind)) {}

ReqPtr makeReqGetFileDetail(std::string path) {
  return std::make_shared<Req>(request::GetFileDetail{std::move(path)});
}
ReqPtr makeReqGetFileList(std::string path) {
  return std::make_shared<Req>(request::GetFileList{std::move(path)});
}
Json::Value Req::toJson() {
  Json::Value json;
  json["kind"] = kind.index();
  std::visit(
    overload{
      [&json](const request::GetFileDetail& req) {
        // 获取variant index
        json["payload"]["path"] = req.path;
      },
      [&json](const request::GetFileList& req) {
        json["payload"]["path"] = req.path;
      },
      [](auto&&) { throw std::runtime_error("Unknown request type"); },
    },
    kind
  );
  return json;
}

ReqPtr Req::fromJson(const Json::Value& json) {
  ReqPtr req;
  switch (json["kind"].asInt()) {
    case 0:
      req = makeReqGetFileDetail(json["payload"]["path"].asString());
      break;
    case 1:
      req = makeReqGetFileList(json["payload"]["path"].asString());
      break;
    default:
      break;
  }
  return req;
}
}  // namespace zipfiles