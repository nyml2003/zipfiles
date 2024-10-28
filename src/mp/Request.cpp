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
      [&json](request::GetCommitDetail& req) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_COMMIT_DETAIL);
        json["payload"]["uuid"] = req.uuid;
      },
      [&json](request::GetCommitList&) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_COMMIT_LIST);
      },
      [&json](request::GetFileList& req) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
        json["payload"]["path"] = req.path;
      },
      [&json](request::GetFileDetailList& req) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILEDETAIL_LIST);
        json["payload"]["path"] = req.path;
        if (!req.filter.has_value()) {
          return;
        }
        auto& filter = req.filter.value();
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
        if (filter.minCreateTime.has_value()) {
          json["payload"]["filter"]["minCreateTime"] =
            filter.minCreateTime.value();
        }
        if (filter.maxCreateTime.has_value()) {
          json["payload"]["filter"]["maxCreateTime"] =
            filter.maxCreateTime.value();
        }
        if (filter.minUpdateTime.has_value()) {
          json["payload"]["filter"]["minUpdateTime"] =
            filter.minUpdateTime.value();
        }
        if (filter.maxUpdateTime.has_value()) {
          json["payload"]["filter"]["maxUpdateTime"] =
            filter.maxUpdateTime.value();
        }
        if (filter.owner) {
          json["payload"]["filter"]["owner"] = filter.owner.value();
        }
        if (filter.group) {
          json["payload"]["filter"]["group"] = filter.group.value();
        }
      },
      [&json](request::PostCommit& req) {
        json["apiEnum"] = toSizeT(ApiEnum::POST_COMMIT);
        json["payload"]["files"] = Json::arrayValue;
        for (const auto& file : req.files) {
          json["payload"]["files"].append(file);
        }
        json["payload"]["uuid"] = req.uuid;
        json["payload"]["message"] = req.message;
        json["payload"]["createTime"] = req.createTime;
        json["payload"]["storagePath"] = req.storagePath;
        json["payload"]["author"] = req.author;
        json["payload"]["isEncrypt"] = req.isEncrypt;
        if (req.key) {
          json["payload"]["key"] = req.key.value();
        }
      },
      [&json](request::MockNeedTime& req) {
        json["apiEnum"] = toSizeT(ApiEnum::MOCK_NEED_TIME);
        json["payload"]["id"] = req.id;
      },
      [&json](request::GetFileDetail& req) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_DETAIL);
        json["payload"]["path"] = req.path;
        json["payload"]["name"] = req.name;
      },
      [](auto&&) { throw std::runtime_error("Unknown request type"); },
    },
    kind
  );
  return json;
}

ReqPtr Req::fromJson(const Json::Value& json) {
  ReqPtr req = nullptr;
  auto api = static_cast<ApiEnum>(json["apiEnum"].asInt());
  log4cpp::Category::getRoot().infoStream()
    << "Making a request from json: " << json << toSizeT(api);
  switch (api) {
    case ApiEnum::GET_COMMIT_DETAIL: {
      req = std::make_shared<Req>(
        request::GetCommitDetail{json["payload"]["uuid"].asString()}
      );
      break;
    }
    case ApiEnum::GET_COMMIT_LIST: {
      req = std::make_shared<Req>(request::GetCommitList{});
      break;
    }
    case ApiEnum::GET_FILE_LIST: {
      req = std::make_shared<Req>(
        request::GetFileList{json["payload"]["path"].asString()}
      );
      break;
    }
    case ApiEnum::GET_FILEDETAIL_LIST: {
      auto path = json["payload"]["path"].asString();
      request::getFileDetailList::Filter filter;
      if (json["payload"]["filter"].isNull()) {
        break;
      }
      const auto& filterJson = json["payload"]["filter"];
      if (!filterJson["type"].isNull()) {
        filter.type = static_cast<fs::file_type>(filterJson["type"].asInt());
      }
      if (!filterJson["minSize"].isNull()) {
        filter.minSize = filterJson["minSize"].asInt();
      }
      if (!filterJson["maxSize"].isNull()) {
        filter.maxSize = filterJson["maxSize"].asInt();
      }
      if (!filterJson["minCreateTime"].isNull()) {
        filter.minCreateTime = filterJson["minCreateTime"].asDouble();
      }
      if (!filterJson["maxCreateTime"].isNull()) {
        filter.maxCreateTime = filterJson["maxCreateTime"].asDouble();
      }
      if (!filterJson["minUpdateTime"].isNull()) {
        filter.minUpdateTime = filterJson["minUpdateTime"].asDouble();
      }
      if (!filterJson["maxUpdateTime"].isNull()) {
        filter.maxUpdateTime = filterJson["maxUpdateTime"].asDouble();
      }
      if (!filterJson["owner"].isNull()) {
        filter.owner = filterJson["owner"].asString();
      }
      if (!filterJson["group"].isNull()) {
        filter.group = filterJson["group"].asString();
      }
      req = std::make_shared<Req>(request::GetFileDetailList{path, filter});
      log4cpp::Category::getRoot().infoStream()
        << "Made a file detail list request from json: " << req->toJson();
      break;
    }
    case ApiEnum::POST_COMMIT: {
      std::vector<std::string> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(file.asString());
      }
      req = std::make_shared<Req>(request::PostCommit{
        files, json["payload"]["uuid"].asString(),
        json["payload"]["message"].asString(),
        json["payload"]["createTime"].asDouble(),
        json["payload"]["storagePath"].asString(),
        json["payload"]["author"].asString(),
        json["payload"]["isEncrypt"].asBool(), json["payload"]["key"].asString()
      });
      break;
    }
    case ApiEnum::GET_FILE_DETAIL: {
      req = std::make_shared<Req>(request::GetFileDetail{
        json["payload"]["path"].asString(), json["payload"]["name"].asString()
      });
      break;
    } break;
    case ApiEnum::MOCK_NEED_TIME: {
      req = std::make_shared<Req>(
        request::MockNeedTime{json["payload"]["id"].asInt()}
      );
      break;
    }
    default:
      break;
  }
  req->timestamp = json["timestamp"].asDouble();
  req->uuid = json["uuid"].asString();
  log4cpp::Category::getRoot().infoStream()
    << "Made a request from json: " << req->toJson();
  return req;
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

}  // namespace zipfiles