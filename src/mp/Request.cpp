#include <utility>
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/common.h"

namespace zipfiles {
Req::Req(ReqKind kind) : kind(std::move(kind)) {}

ReqPtr makeReqGetFileDetail(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail";
  return std::make_shared<Req>(request::GetFileDetail{std::move(path)});
}
ReqPtr makeReqGetFileList(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file list";
  return std::make_shared<Req>(request::GetFileList{std::move(path)});
}

ReqPtr makeReqPostCommit(CommitLog commitLog) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to post commit";
  return std::make_shared<Req>(request::PostCommit{std::move(commitLog)});
}

ReqPtr makeReqMockNeedTime(int id) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to mock need time";
  return std::make_shared<Req>(request::MockNeedTime{id});
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
      [&json](request::MockNeedTime& req) {
        json["payload"]["id"] = req.id;
        json["apiEnum"] = toSizeT(ApiEnum::IGNORE);
      },
      [&json](request::PostCommit& req) {
        json["apiEnum"] = toSizeT(ApiEnum::POST_COMMIT);
        json["payload"]["message"] = req.commitLog.message;
        json["payload"]["createTime"] = req.commitLog.createTime;
        json["payload"]["defaultPath"] = req.commitLog.defaultPath;
        json["payload"]["storagePath"] = req.commitLog.storagePath;
        json["payload"]["author"] = req.commitLog.author;
        json["payload"]["isEncrypt"] = req.commitLog.isEncrypt;
        json["payload"]["isDelete"] = req.commitLog.isDelete;
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
    case ApiEnum::IGNORE:
      req = makeReqMockNeedTime(json["payload"]["id"].asInt());
      break;
    default:
      break;
  }
  req->timestamp = json["timestamp"].asDouble();
  req->uuid = json["uuid"].asString();
  return req;
}
}  // namespace zipfiles