#include "mp/Request.h"
#include <optional>
#include <utility>
#include "log4cpp/Category.hh"
#include "mp/Response.h"
#include "mp/common.h"

namespace zipfiles {
Req::Req(ReqKind kind) : kind(std::move(kind)) {}

Json::Value Req::toJson() {
  Json::Value json;
  json["timestamp"] = timestamp;
  json["uuid"] = uuid;
  std::visit(
    overload{
      [&json](request::GetFileDetail& req) {
        json["payload"]["path"] = req.path;
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_DETAIL);
      },
      [&json](request::GetFileList& req) {
        json["payload"]["path"] = req.path;
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
        if (req.filter.has_value()) {
          Filter filter = req.filter.value();
          if (filter.type.has_value()) {
            json["payload"]["filter"]["type"] =
              static_cast<int>(filter.type.value());
          }
          if (filter.minSize.has_value()) {
            json["payload"]["filter"]["minSize"] = filter.minSize.value();
          }
          if (filter.maxSize.has_value()) {
            json["payload"]["filter"]["maxSize"] = filter.maxSize.value();
          }
          if (filter.owner.has_value()) {
            json["payload"]["filter"]["owner"] = filter.owner.value();
          }
          if (filter.group.has_value()) {
            json["payload"]["filter"]["group"] = filter.group.value();
          }
        }
      },
      [&json](request::MockNeedTime& req) {
        json["payload"]["id"] = req.id;
        json["apiEnum"] = toSizeT(ApiEnum::MOCK_NEED_TIME);
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
      [&json](request::GetAllFileDetails& req) {
        json["payload"]["path"] = req.path;
        json["apiEnum"] = toSizeT(ApiEnum::GET_ALL_FILE_DETAILS);
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
    case ApiEnum::GET_FILE_LIST: {
      if (json["payload"]["filter"].isNull()) {
        req = makeReqGetFileList(json["payload"]["path"].asString());
        break;
      }
      Filter filter;
      if (!json["payload"]["filter"]["type"].isNull()) {
        filter.type =
          static_cast<fs::file_type>(json["payload"]["filter"]["type"].asInt());
      }
      if (!json["payload"]["filter"]["minSize"].isNull()) {
        filter.minSize = json["payload"]["filter"]["minSize"].asInt();
      }
      if (!json["payload"]["filter"]["maxSize"].isNull()) {
        filter.maxSize = json["payload"]["filter"]["maxSize"].asInt();
      }
      if (!json["payload"]["filter"]["owner"].isNull()) {
        filter.owner = json["payload"]["filter"]["owner"].asString();
      }
      if (!json["payload"]["filter"]["group"].isNull()) {
        filter.group = json["payload"]["filter"]["group"].asString();
      }
      req = makeReqGetFileList(json["payload"]["path"].asString(), filter);
      break;
    }
    case ApiEnum::MOCK_NEED_TIME:
      req = makeReqMockNeedTime(json["payload"]["id"].asInt());
      break;
    case ApiEnum::POST_COMMIT:
      req = makeReqPostCommit(CommitLog{
        json["payload"]["uuid"].asString(),
        json["payload"]["message"].asString(),
        json["payload"]["createTime"].asDouble(),
        json["payload"]["defaultPath"].asString(),
        json["payload"]["storagePath"].asString(),
        json["payload"]["author"].asString(),
        json["payload"]["isEncrypt"].asBool(),
        json["payload"]["isDelete"].asBool(),
      });
      break;
    case ApiEnum::GET_ALL_FILE_DETAILS:
      req = makeReqGetAllFileDetails(json["payload"]["path"].asString());
      break;
    default:
      break;
  }
  req->timestamp = json["timestamp"].asDouble();
  req->uuid = json["uuid"].asString();
  return req;
}

ReqPtr makeReqGetFileDetail(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail";
  return std::make_shared<Req>(request::GetFileDetail{std::move(path)});
}
/**
 * @brief
 * 通过fromJson来还原一个ReqPtr，代价是会丢失Req的timestamp和uuid
 * @param payload
 * @return ReqPtr
 */
ReqPtr makeReqGetFileDetail(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_DETAIL);
  return Req::fromJson(json);
}

ReqPtr makeReqGetFileList(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file list";
  return std::make_shared<Req>(
    request::GetFileList{std::move(path), std::nullopt}
  );
}

ReqPtr makeReqGetFileList(std::string path, Filter filter) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file list";
  return std::make_shared<Req>(
    request::GetFileList{std::move(path), std::move(filter)}
  );
}

ReqPtr makeReqGetFileList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
  return Req::fromJson(json);
}

ReqPtr makeReqPostCommit(CommitLog commitLog) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to post commit";
  return std::make_shared<Req>(request::PostCommit{std::move(commitLog)});
}

ReqPtr makeReqPostCommit(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to post commit";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::POST_COMMIT);
  return Req::fromJson(json);
}

ReqPtr makeReqMockNeedTime(int id) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to mock need time";
  return std::make_shared<Req>(request::MockNeedTime{id});
}

ReqPtr makeReqMockNeedTime(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to mock need time";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::MOCK_NEED_TIME);
  return Req::fromJson(json);
}

ReqPtr makeReqGetAllFileDetails(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get all file details";
  return std::make_shared<Req>(request::GetAllFileDetails{std::move(path)});
}

ReqPtr makeReqGetAllFileDetails(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get all file details";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_ALL_FILE_DETAILS);
  return Req::fromJson(json);
}
}  // namespace zipfiles