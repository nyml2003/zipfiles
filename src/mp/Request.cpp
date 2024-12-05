#include "mp/Request.h"

#include <optional>
#include <utility>

#include "mp/common.h"

namespace zipfiles {

Api matchApi(ReqKind kind) {
  return std::visit(
    overload{
      [](const request::GetCommitDetail&) { return Api::GET_COMMIT_DETAIL; },
      [](const request::GetCommitList&) { return Api::GET_COMMIT_LIST; },
      [](const request::GetFileList&) { return Api::GET_FILE_LIST; },
      [](const request::GetFileDetail&) { return Api::GET_FILE_DETAIL; },
      [](const request::GetFileDetailList&) {
        return Api::GET_FILE_DETAIL_LIST;
      },
      [](const request::PostCommit&) { return Api::POST_COMMIT; },
      [](const request::Restore&) { return Api::RESTORE; },
      [](const request::LogicDeleteCommit&) {
        return Api::LOGIC_DELETE_COMMIT;
      },
      [](const request::PhysicalDeleteCommit&) {
        return Api::PHYSICAL_DELETE_COMMIT;
      },
      [](const request::GetCommitRecycleBin&) {
        return Api::GET_COMMIT_RECYCLE_BIN;
      },
      [](const request::RecoverCommit&) { return Api::RECOVER_COMMIT; },
      [](const request::MockNeedTime&) { return Api::MOCK_NEED_TIME; },
      [](const request::MockManyNotifications&) {
        return Api::MOCK_MANY_NOTIFICATIONS;
      },
      [](const auto&) {
        throw UnknownApiException("错误发生在: Request::matchApi");
        return Api::ILLEAGAL;
      }
    }  // namespace zipfiles
    ,
    kind
  );
}

Req::Req(ReqKind kind, std::string uuid)
  : kind(std::move(kind)), uuid(std::move(uuid)) {
  api = matchApi(this->kind);
}

Json::Value Req::toJson() const {
  Json::Value json;
  json["uuid"] = uuid;
  json["api"] = static_cast<int>(api);
  switch (api) {
    case Api::GET_COMMIT_DETAIL: {
      json["payload"]["uuid"] = std::get<request::GetCommitDetail>(kind).uuid;
      break;
    }
    case Api::GET_COMMIT_LIST: {
      break;
    }
    case Api::GET_FILE_LIST: {
      json["payload"]["path"] = std::get<request::GetFileList>(kind).path;
      break;
    }
    case Api::GET_FILE_DETAIL_LIST: {
      json["payload"]["path"] = std::get<request::GetFileDetailList>(kind).path;
      if (std::get<request::GetFileDetailList>(kind).filter.has_value()) {
        const auto& filter =
          std::get<request::GetFileDetailList>(kind).filter.value();
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
      }
      break;
    }
    case Api::POST_COMMIT: {
      json["payload"]["files"] = Json::arrayValue;
      for (const auto& file : std::get<request::PostCommit>(kind).files) {
        json["payload"]["files"].append(file);
      }
      json["payload"]["uuid"] = std::get<request::PostCommit>(kind).uuid;
      json["payload"]["message"] = std::get<request::PostCommit>(kind).message;
      json["payload"]["createTime"] =
        std::get<request::PostCommit>(kind).createTime;
      json["payload"]["storagePath"] =
        std::get<request::PostCommit>(kind).storagePath;
      json["payload"]["author"] = std::get<request::PostCommit>(kind).author;
      json["payload"]["isEncrypt"] =
        std::get<request::PostCommit>(kind).isEncrypt;
      if (std::get<request::PostCommit>(kind).key) {
        json["payload"]["key"] =
          std::get<request::PostCommit>(kind).key.value();
      }
      break;
    }
    case Api::GET_FILE_DETAIL: {
      json["payload"]["path"] = std::get<request::GetFileDetail>(kind).path;
      json["payload"]["name"] = std::get<request::GetFileDetail>(kind).name;
      break;
    }
    case Api::RESTORE: {
      const auto& restore = std::get<request::Restore>(kind);
      if (restore.key.has_value()) {
        json["payload"]["key"] = restore.key.value();
      }
      json["payload"]["commitId"] = restore.commitId;
      json["payload"]["messageId"] = restore.messageId;
      json["payload"]["path"] = restore.path;
      break;
    }
    case Api::MOCK_NEED_TIME: {
      json["payload"]["id"] = std::get<request::MockNeedTime>(kind).id;
      break;
    }
    case Api::GET_COMMIT_RECYCLE_BIN: {
      break;
    }
    case Api::LOGIC_DELETE_COMMIT: {
      json["payload"]["commitId"] =
        std::get<request::LogicDeleteCommit>(kind).commitId;
      break;
    }
    case Api::PHYSICAL_DELETE_COMMIT: {
      json["payload"]["commitId"] =
        std::get<request::PhysicalDeleteCommit>(kind).commitId;
      break;
    }
    case Api::RECOVER_COMMIT: {
      json["payload"]["commitId"] =
        std::get<request::RecoverCommit>(kind).commitId;
      break;
    }
    case Api::MOCK_MANY_NOTIFICATIONS: {
      break;
    }
    default:
      throw UnknownApiException(
        "错误发生在: Request::toJson, 未知的API类型: " +
        std::to_string(static_cast<int>(api))
      );
      break;
  }
  return json;
}

Req Req::fromJson(const Json::Value& json) {
  auto api = static_cast<Api>(json["api"].asInt());
  ReqKind kind;
  switch (api) {
    case Api::GET_COMMIT_DETAIL: {
      kind =
        request::GetCommitDetail{.uuid = json["payload"]["uuid"].asString()};
      break;
    }
    case Api::GET_COMMIT_LIST: {
      kind = request::GetCommitList{};
      break;
    }
    case Api::GET_FILE_LIST: {
      kind = request::GetFileList{.path = json["payload"]["path"].asString()};
      break;
    }
    case Api::GET_FILE_DETAIL_LIST: {
      auto path = json["payload"]["path"].asString();
      request::getFileDetailList::Filter filter;
      if (json["payload"]["filter"].isNull()) {
        kind = request::GetFileDetailList{.path = path};
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
      kind = request::GetFileDetailList{.path = path, .filter = filter};
      break;
    }
    case Api::POST_COMMIT: {
      std::vector<std::string> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(file.asString());
      }
      kind = request::PostCommit{
        .files = files,
        .uuid = json["payload"]["uuid"].asString(),
        .message = json["payload"]["message"].asString(),
        .createTime = json["payload"]["createTime"].asDouble(),
        .storagePath = json["payload"]["storagePath"].asString(),
        .author = json["payload"]["author"].asString(),
        .isEncrypt = json["payload"]["isEncrypt"].asBool(),
        .key = json["payload"]["key"].asString()
      };
      break;
    }
    case Api::GET_FILE_DETAIL: {
      kind = request::GetFileDetail{
        .path = json["payload"]["path"].asString(),
        .name = json["payload"]["name"].asString()
      };
      break;
    }
    case Api::RESTORE: {
      kind = request::Restore{
        .key =
          json["payload"]["key"].isNull()
            ? std::nullopt
            : std::optional<std::string>(json["payload"]["key"].asString()),
        .commitId = json["payload"]["commitId"].asString(),
        .path = json["payload"]["path"].asString(),
        .messageId = json["payload"]["messageId"].asString(),
      };
      break;
    }
    case Api::MOCK_NEED_TIME: {
      kind = request::MockNeedTime{.id = json["payload"]["id"].asInt()};
      break;
    };
    case Api::MOCK_MANY_NOTIFICATIONS:
      kind = request::MockManyNotifications{};
      break;
    case Api::GET_COMMIT_RECYCLE_BIN:
      kind = request::GetCommitRecycleBin{};
      break;
    case Api::LOGIC_DELETE_COMMIT: {
      kind = request::LogicDeleteCommit{
        .commitId = json["payload"]["commitId"].asString()
      };
      break;
    }
    case Api::PHYSICAL_DELETE_COMMIT: {
      kind = request::PhysicalDeleteCommit{
        .commitId = json["payload"]["commitId"].asString()
      };
      break;
    }
    case Api::RECOVER_COMMIT: {
      kind = request::RecoverCommit{
        .commitId = json["payload"]["commitId"].asString()
      };
      break;
    }
    default:
      throw UnknownApiException(
        "错误发生在: Request::fromJson, 未知的API类型: " +
        std::to_string(static_cast<int>(api))
      );
      break;
  }
  return {kind, json["uuid"].asString()};
}

}  // namespace zipfiles