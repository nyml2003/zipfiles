#include <unistd.h>
#include <filesystem>
#include <log4cpp/Category.hh>
#include <utility>
#include <vector>
#include "json/value.h"
#include "mp/Response.h"
#include "mp/common.h"

namespace zipfiles {
namespace fs = std::filesystem;
Res::Res(ResKind kind) : kind(std::move(kind)) {}

std::ostream& operator<<(std::ostream& os, const StatusCode& status) {
  switch (status) {
    case StatusCode::OK:
      os << "OK";
      break;
    case StatusCode::ERROR:
      os << "ERROR";
      break;
    // 其他状态码...
    default:
      os << "Unknown Status";
      break;
  }
  return os;
}

size_t toSizeT(ApiEnum apiEnum) {
  return static_cast<size_t>(apiEnum);
}
Json::Value Res::toJson() {
  Json::Value json;
  json["status"] = static_cast<int>(status);
  json["timestamp"] = timestamp;
  json["uuid"] = uuid;
  std::visit(
    overload{
      [&json](response::GetCommitDetail& res) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_COMMIT_DETAIL);
        json["payload"]["files"] = Json::arrayValue;
        for (const auto& file : res.files) {
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
      },
      [&json](response::GetCommitList& res) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_COMMIT_LIST);
        Json::Value files(Json::arrayValue);
        for (const auto& file : res.commits) {
          Json::Value fileJson;
          fileJson["uuid"] = file.uuid;
          fileJson["message"] = file.message;
          fileJson["createTime"] = file.createTime;
          fileJson["storagePath"] = file.storagePath;
          fileJson["author"] = file.author;
          fileJson["isEncrypt"] = file.isEncrypt;
          fileJson["isDelete"] = file.isDelete;
          files.append(fileJson);
        }
        json["payload"]["files"] = files;
      },
      [&json](response::GetFileList& res) {
        json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
        Json::Value files(Json::arrayValue);
        for (const auto& file : res.files) {
          Json::Value fileJson;
          fileJson["name"] = file.name;
          fileJson["type"] = static_cast<int>(file.type);
          files.append(fileJson);
        }
        json["payload"]["files"] = files;
      },
      [&json](response::MockNeedTime& res) {
        json["apiEnum"] = toSizeT(ApiEnum::MOCK_NEED_TIME);
        json["payload"]["id"] = res.id;
      },
      [&json](response::PostCommit&) {
        json["apiEnum"] = toSizeT(ApiEnum::POST_COMMIT);
      },
      [](auto&&) { throw std::runtime_error("Unknown response type"); }
    },
    kind
  );
  return json;
}

ResPtr Res::fromJson(const Json::Value& json) {
  ResPtr res;
  auto api = static_cast<ApiEnum>(json["apiEnum"].asInt());
  switch (api) {
    case ApiEnum::GET_COMMIT_DETAIL: {
      std::vector<response::getCommitDetail::FileDetail> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(response::getCommitDetail::FileDetail{
          static_cast<fs::file_type>(file["type"].asInt()),
          file["createTime"].asDouble(), file["updateTime"].asDouble(),
          file["size"].asInt(), file["owner"].asString(),
          file["group"].asString(), static_cast<mode_t>(file["mode"].asInt()),
          file["path"].asString(), file["name"].asString()
        });
      }
      res = std::make_shared<Res>(response::GetCommitDetail{files});
      break;
    }
    case ApiEnum::GET_COMMIT_LIST: {
      std::vector<response::getCommitList::CommitLog> commits;
      for (const auto& file : json["payload"]["files"]) {
        commits.push_back(response::getCommitList::CommitLog{
          file["uuid"].asString(), file["message"].asString(),
          file["createTime"].asDouble(), file["defaultPath"].asString(),
          file["storagePath"].asString(), file["author"].asString(),
          file["isEncrypt"].asBool(), file["isDelete"].asBool()
        });
      }
      res = std::make_shared<Res>(response::GetCommitList{commits});
      break;
    }
    case ApiEnum::GET_FILE_LIST: {
      std::vector<response::getFileList::File> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(response::getFileList::File{
          static_cast<fs::file_type>(file["type"].asInt()),
          file["name"].asString()
        });
      }
      res = std::make_shared<Res>(response::GetFileList{files});
      break;
    }
    case ApiEnum::POST_COMMIT:
      res = std::make_shared<Res>(response::PostCommit{});
      break;
    case ApiEnum::MOCK_NEED_TIME:
      res = std::make_shared<Res>(
        response::MockNeedTime{json["payload"]["id"].asInt()}
      );
      break;
    default:
      throw std::runtime_error("Invalid response kind");
      break;
  }
  res->status = static_cast<StatusCode>(json["status"].asInt());
  res->timestamp = json["timestamp"].asDouble();
  res->uuid = json["uuid"].asString();
  return res;
}

ResPtr makeResMockNeedTime(int id) {
  sleep(10);
  return std::make_shared<Res>(response::MockNeedTime{id});
}

ResPtr makeResMockNeedTime(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to mock need time";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::MOCK_NEED_TIME);
  return Res::fromJson(json);
}

}  // namespace zipfiles