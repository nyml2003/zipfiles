#include "mp/Response.h"

#include <json/json.h>
#include <unistd.h>
#include <filesystem>
#include <log4cpp/Category.hh>
#include <vector>

#include "mp/common.h"

namespace zipfiles {

Api matchApi(ResKind kind) {
  return std::visit(
    overload{
      [](const response::GetCommitDetail&) { return Api::GET_COMMIT_DETAIL; },
      [](const response::GetCommitList&) { return Api::GET_COMMIT_LIST; },
      [](const response::GetFileList&) { return Api::GET_FILE_LIST; },
      [](const response::GetFileDetail&) { return Api::GET_FILE_DETAIL; },
      [](const response::GetFileDetailList&) {
        return Api::GET_FILE_DETAIL_LIST;
      },
      [](const response::PostCommit&) { return Api::POST_COMMIT; },
      [](const response::Restore&) { return Api::RESTORE; },
      [](const response::LogicDeleteCommit&) {
        return Api::LOGIC_DELETE_COMMIT;
      },
      [](const response::PhysicalDeleteCommit&) {
        return Api::PHYSICAL_DELETE_COMMIT;
      },
      [](const response::GetCommitRecycleBin&) {
        return Api::GET_COMMIT_RECYCLE_BIN;
      },
      [](const response::RecoverCommit&) { return Api::RECOVER_COMMIT; },
      [](const response::MockNeedTime&) { return Api::MOCK_NEED_TIME; },
      [](const response::MockManyNotifications&) {
        return Api::MOCK_MANY_NOTIFICATIONS;
      },
      [](const response::NoResponse&) { return Api::NORESPONSE; },
      [](const response::CreateNullFolder&) { return Api::CREATE_NULL_FOLDER; },
      [](const auto&) {
        throw UnknownApiException("错误发生在: Response::matchApi");
        return Api::ILLEAGAL;
      }},
    kind
  );
}

Res::Res(ResKind kind, std::string uuid, Code code)
  : kind(std::move(kind)), uuid(std::move(uuid)), code(code) {
  api = matchApi(this->kind);
}

Json::Value Res::toJson() const {
  Json::Value json;
  json["api"] = static_cast<int>(api);
  json["code"] = static_cast<int>(code);
  json["uuid"] = uuid;
  switch (api) {
    case Api::GET_COMMIT_DETAIL: {
      json["payload"]["files"] = Json::arrayValue;
      for (const auto& file : std::get<response::GetCommitDetail>(kind).files) {
        Json::Value fileJson;
        fileJson["type"] = static_cast<int>(file.type);
        fileJson["createTime"] = file.createTime;
        fileJson["updateTime"] = file.updateTime;
        fileJson["size"] = file.size;
        fileJson["owner"] = file.owner;
        fileJson["group"] = file.group;
        fileJson["mode"] = file.mode;
        fileJson["path"] = file.path;
        fileJson["name"] = file.name;
        json["payload"]["files"].append(fileJson);
      }
      break;
    }
    case Api::GET_COMMIT_LIST: {
      Json::Value commits(Json::arrayValue);
      for (const auto& commit :
           std::get<response::GetCommitList>(kind).commits) {
        Json::Value commitJson;
        commitJson["uuid"] = commit.uuid;
        commitJson["message"] = commit.message;
        commitJson["createTime"] = commit.createTime;
        commitJson["storagePath"] = commit.storagePath;
        commitJson["author"] = commit.author;
        commitJson["isEncrypt"] = commit.isEncrypt;
        commitJson["isDelete"] = commit.isDelete;
        commits.append(commitJson);
      }
      json["payload"]["commits"] = commits;
      break;
    }
    case Api::GET_FILE_LIST: {
      Json::Value files(Json::arrayValue);
      for (const auto& file : std::get<response::GetFileList>(kind).files) {
        Json::Value fileJson;
        fileJson["name"] = file.name;
        fileJson["type"] = static_cast<int>(file.type);
        files.append(fileJson);
      }
      json["payload"]["files"] = files;
      break;
    }
    case Api::POST_COMMIT: {
      break;
    }
    case Api::GET_FILE_DETAIL: {
      json["payload"]["type"] =
        static_cast<int>(std::get<response::GetFileDetail>(kind).type);
      json["payload"]["createTime"] =
        std::get<response::GetFileDetail>(kind).createTime;
      json["payload"]["updateTime"] =
        std::get<response::GetFileDetail>(kind).updateTime;
      json["payload"]["size"] = std::get<response::GetFileDetail>(kind).size;
      json["payload"]["owner"] = std::get<response::GetFileDetail>(kind).owner;
      json["payload"]["group"] = std::get<response::GetFileDetail>(kind).group;
      json["payload"]["mode"] = std::get<response::GetFileDetail>(kind).mode;
      json["payload"]["path"] = std::get<response::GetFileDetail>(kind).path;
      json["payload"]["name"] = std::get<response::GetFileDetail>(kind).name;
      break;
    }
    case Api::GET_FILE_DETAIL_LIST: {
      Json::Value files(Json::arrayValue);
      for (const auto& file :
           std::get<response::GetFileDetailList>(kind).files) {
        Json::Value fileJson;
        fileJson["type"] = static_cast<int>(file.type);
        fileJson["createTime"] = file.createTime;
        fileJson["updateTime"] = file.updateTime;
        fileJson["size"] = file.size;
        fileJson["owner"] = file.owner;
        fileJson["group"] = file.group;
        fileJson["mode"] = file.mode;
        fileJson["path"] = file.path;
        fileJson["name"] = file.name;
        files.append(fileJson);
      }
      json["payload"]["files"] = files;
      break;
    }
    case Api::RESTORE: {
      break;
    }
    case Api::GET_COMMIT_RECYCLE_BIN: {
      Json::Value commits(Json::arrayValue);
      for (const auto& commit :
           std::get<response::GetCommitRecycleBin>(kind).commits) {
        Json::Value commitJson;
        commitJson["uuid"] = commit.uuid;
        commitJson["message"] = commit.message;
        commitJson["createTime"] = commit.createTime;
        commitJson["storagePath"] = commit.storagePath;
        commitJson["author"] = commit.author;
        commitJson["isEncrypt"] = commit.isEncrypt;
        commitJson["isDelete"] = commit.isDelete;
        commits.append(commitJson);
      }
      json["payload"]["commits"] = commits;
      break;
    }
    case Api::LOGIC_DELETE_COMMIT: {
      break;
    }
    case Api::RECOVER_COMMIT: {
      break;
    }
    case Api::PHYSICAL_DELETE_COMMIT: {
      break;
    }
    case Api::MOCK_NEED_TIME: {
      json["payload"]["id"] = std::get<response::MockNeedTime>(kind).id;
      break;
    }
    case Api::MOCK_MANY_NOTIFICATIONS: {
      break;
    }
    case Api::CREATE_NULL_FOLDER: {
      break;
    }
    case Api::NORESPONSE: {
      response::NoResponse noResponse = std::get<response::NoResponse>(kind);
      json["payload"]["title"] = noResponse.title;
      json["payload"]["description"] = noResponse.description;
      break;
    }
    default:
      throw UnknownApiException(
        "错误发生在: Response::toJson, 未知的API类型: " +
        std::to_string(static_cast<int>(api))
      );
  }

  return json;
}

Res Res::fromJson(const Json::Value& json) {
  auto api = static_cast<Api>(json["api"].asInt());
  ResKind kind;
  switch (api) {
    case Api::GET_COMMIT_DETAIL: {
      std::vector<response::getCommitDetail::FileDetail> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(response::getCommitDetail::FileDetail{
          .type = static_cast<fs::file_type>(file["type"].asInt()),
          .createTime = file["createTime"].asDouble(),
          .updateTime = file["updateTime"].asDouble(),
          .size = file["size"].asInt(),
          .owner = file["owner"].asString(),
          .group = file["group"].asString(),
          .mode = static_cast<mode_t>(file["mode"].asInt()),
          .path = file["path"].asString(),
          .name = file["name"].asString()});
      }
      kind = response::GetCommitDetail{.files = files};
      break;
    }
    case Api::GET_COMMIT_LIST: {
      std::vector<response::getCommitList::CommitLog> commits;
      for (const auto& file : json["payload"]["files"]) {
        commits.push_back(
          {.uuid = file["uuid"].asString(),
           .message = file["message"].asString(),
           .createTime = file["createTime"].asDouble(),
           .storagePath = file["storagePath"].asString(),
           .author = file["author"].asString(),
           .isEncrypt = file["isEncrypt"].asBool(),
           .isDelete = file["isDelete"].asBool()}
        );
      }
      kind = response::GetCommitList{.commits = commits};
      break;
    }
    case Api::GET_FILE_LIST: {
      std::vector<response::getFileList::File> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(response::getFileList::File{
          .type = static_cast<fs::file_type>(file["type"].asInt()),
          .name = file["name"].asString()});
      }
      kind = response::GetFileList{.files = files};
      break;
    }
    case Api::POST_COMMIT: {
      kind = response::PostCommit{};
      break;
    }
    case Api::GET_FILE_DETAIL: {
      kind = response::GetFileDetail(
        {.type = static_cast<fs::file_type>(json["payload"]["type"].asInt()),
         .createTime = json["payload"]["createTime"].asDouble(),
         .updateTime = json["payload"]["updateTime"].asDouble(),
         .size = json["payload"]["size"].asInt(),
         .owner = json["payload"]["owner"].asString(),
         .group = json["payload"]["group"].asString(),
         .mode = static_cast<mode_t>(json["payload"]["mode"].asInt()),
         .path = json["payload"]["path"].asString(),
         .name = json["payload"]["name"].asString()}
      );
      break;
    }
    case Api::GET_FILE_DETAIL_LIST: {
      std::vector<response::getFileDetailList::FileDetail> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(response::getFileDetailList::FileDetail{
          .type = static_cast<fs::file_type>(file["type"].asInt()),
          .createTime = file["createTime"].asDouble(),
          .updateTime = file["updateTime"].asDouble(),
          .size = file["size"].asInt(),
          .owner = file["owner"].asString(),
          .group = file["group"].asString(),
          .mode = static_cast<mode_t>(file["mode"].asInt()),
          .path = file["path"].asString(),
          .name = file["name"].asString()});
      }
      kind = response::GetFileDetailList{.files = files};
      break;
    }
    case Api::RESTORE: {
      kind = response::Restore{};
      break;
    }
    case Api::LOGIC_DELETE_COMMIT: {
      kind = response::LogicDeleteCommit{};
      break;
    }
    case Api::PHYSICAL_DELETE_COMMIT: {
      kind = response::PhysicalDeleteCommit{};
      break;
    }
    case Api::GET_COMMIT_RECYCLE_BIN: {
      std::vector<response::getCommitRecycleBin::CommitLog> commits;
      for (const auto& file : json["payload"]["commits"]) {
        commits.push_back(
          {.uuid = file["uuid"].asString(),
           .message = file["message"].asString(),
           .createTime = file["createTime"].asDouble(),
           .storagePath = file["storagePath"].asString(),
           .author = file["author"].asString(),
           .isEncrypt = file["isEncrypt"].asBool(),
           .isDelete = file["isDelete"].asBool()}
        );
      }
      kind = response::GetCommitRecycleBin{.commits = commits};
      break;
    }
    case Api::RECOVER_COMMIT: {
      kind = response::RecoverCommit{};
      break;
    }
    case Api::MOCK_NEED_TIME: {
      kind = response::MockNeedTime{.id = json["payload"]["id"].asInt()};
      break;
    }
    case Api::MOCK_MANY_NOTIFICATIONS: {
      kind = response::MockManyNotifications{};
      break;
    }
    case Api::CREATE_NULL_FOLDER: {
      kind = response::CreateNullFolder{};
      break;
    }
    case Api::NORESPONSE: {
      kind = response::NoResponse{
        .title = json["payload"]["title"].asString(),
        .description = json["payload"]["description"].asString()};
      break;
    }
    default:
      throw UnknownApiException(
        "错误发生在: Response::fromJson, 未知的API类型: " +
        std::to_string(static_cast<int>(api))
      );
      break;
  }
  return {
    kind, json["uuid"].asString(), static_cast<Code>(json["code"].asInt())};
}

}  // namespace zipfiles