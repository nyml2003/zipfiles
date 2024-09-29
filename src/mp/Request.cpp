#include <utility>
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/common.h"

namespace zipfiles {
Req::Req(ReqKind kind) : kind(std::move(kind)) {}

ReqPtr makeReqGetFileDetail(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "make a request to get file detail";
  return std::make_shared<Req>(request::GetFileDetail{std::move(path)});
}
ReqPtr makeReqGetFileList(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "make a request to get file list";
  return std::make_shared<Req>(request::GetFileList{std::move(path)});
}
Json::Value Req::toJson() {
  Json::Value json;
  json["timestamp"] = timestamp;
  json["uuid"] = uuid;
  std::visit(
    overload{
      [&json](request::GetFileDetail& req) {
        // 获取variant index
        json["payload"]["path"] = req.path;
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_DETAIL);
      },
      [&json](request::GetFileList& req) {
        json["payload"]["path"] = req.path;
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
      },
      [](auto&&) { throw std::runtime_error("Unknown request type"); },
    },
    kind
  );
  return json;
}

ReqPtr Req::fromJson(const Json::Value& json) {
  ReqPtr req;
  auto api = static_cast<ApiEnum>(json["apiEnum"].asInt());
  switch (api) {
    case ApiEnum::GET_FILE_DETAIL:
      req = makeReqGetFileDetail(json["payload"]["path"].asString());
      break;
    case ApiEnum::GET_FILE_LIST:
      req = makeReqGetFileList(json["payload"]["path"].asString());
      break;
    default:
      break;
  }
  req->timestamp = json["timestamp"].asDouble();
  req->uuid = json["uuid"].asString();
  return req;
}
}  // namespace zipfiles