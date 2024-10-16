#include "mp/Response.h"
#include <unistd.h>
#include <filesystem>
#include <log4cpp/Category.hh>
#include <utility>
#include <vector>
#include "json/value.h"
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
      [&json](response::GetFileDetail& res) {
        json["apiEnum"] = Json::Value(toSizeT(ApiEnum::GET_FILE_DETAIL));
        const auto& metadata = res.metadata;
        json["payload"]["absolutePath"] = metadata.absolutePath;
        json["payload"]["type"] = static_cast<int>(metadata.type);
        json["payload"]["createTime"] = metadata.createTime;
        json["payload"]["updateTime"] = metadata.updateTime;
        json["payload"]["owner"] = metadata.owner;
        json["payload"]["group"] = metadata.group;
        json["payload"]["mode"] = metadata.mode;
        json["payload"]["size"] = static_cast<Json::UInt64>(metadata.size);
      },
      [&json](response::GetFileList& res) {
        json["apiEnum"] = Json::Value(toSizeT(ApiEnum::GET_FILE_LIST));
        json["payload"]["files"] = Json::arrayValue;
        for (const auto& file : res.files) {
          Json::Value fileJson;
          fileJson["name"] = file.name;
          fileJson["type"] = static_cast<int>(file.type);
          json["payload"]["files"].append(fileJson);
        }
      },
      [&json](response::MockNeedTime& res) {
        json["apiEnum"] = Json::Value(toSizeT(ApiEnum::MOCK_NEED_TIME));
        json["payload"]["id"] = res.id;
      },
      [&json](response::PostCommit&) {
        json["apiEnum"] = Json::Value(toSizeT(ApiEnum::POST_COMMIT));
      },
      [&json](response::GetAllFileDetails& res) {
        json["apiEnum"] = Json::Value(toSizeT(ApiEnum::GET_ALL_FILE_DETAILS));
        json["payload"]["files"] = Json::arrayValue;
        for (const auto& file : res.files) {
          Json::Value fileJson;
          fileJson["type"] = static_cast<int>(file.type);
          fileJson["createTime"] = file.createTime;
          fileJson["updateTime"] = file.updateTime;
          fileJson["size"] = static_cast<Json::UInt64>(file.size);
          fileJson["owner"] = file.owner;
          fileJson["group"] = file.group;
          fileJson["mode"] = file.mode;
          fileJson["absolutePath"] = file.absolutePath;
          json["payload"]["files"].append(fileJson);
        }
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
    case ApiEnum::GET_FILE_DETAIL: {
      FileDetail metadata = {
        static_cast<fs::file_type>(json["payload"]["type"].asInt()),
        json["payload"]["createTime"].asDouble(),
        json["payload"]["updateTime"].asDouble(),
        static_cast<__off_t>(json["payload"]["size"].asUInt64()),
        json["payload"]["owner"].asString(),
        json["payload"]["group"].asString(),
        json["payload"]["mode"].asUInt(),
        json["payload"]["name"].asString(),
      };
      res = makeResGetFileDetail(metadata);
      break;
    }
    case ApiEnum::GET_FILE_LIST: {
      std::vector<File> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(
          {file["name"].asString(),
           static_cast<fs::file_type>(file["type"].asInt())}
        );
      }
      res = makeResGetFileList(files);
      break;
    }
    case ApiEnum::MOCK_NEED_TIME:
      res = makeResMockNeedTime(json["payload"]["id"].asInt());
      break;
    case ApiEnum::POST_COMMIT:
      res = makeResPostCommit();
      break;
    case ApiEnum::GET_ALL_FILE_DETAILS: {
      std::vector<FileDetail> files;
      for (const auto& file : json["payload"]["files"]) {
        files.push_back(
          {static_cast<fs::file_type>(file["type"].asInt()),
           file["createTime"].asDouble(), file["updateTime"].asDouble(),
           static_cast<__off_t>(file["size"].asUInt64()),
           file["owner"].asString(), file["group"].asString(),
           file["mode"].asUInt(), file["name"].asString()}
        );
      }
      res = makeResGetAllFileDetails(files);
      break;
    }

    default:
      throw std::runtime_error("Invalid response kind");
      break;
  }
  res->status = static_cast<StatusCode>(json["status"].asInt());
  res->timestamp = json["timestamp"].asDouble();
  res->uuid = json["uuid"].asString();
  return res;
}
ResPtr makeResGetFileDetail(FileDetail metadata) {
  return std::make_shared<Res>(response::GetFileDetail{std::move(metadata)});
}

ResPtr makeResGetFileDetail(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file detail";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_DETAIL);
  return Res::fromJson(json);
}

ResPtr makeResGetFileList(std::vector<File> files) {
  return std::make_shared<Res>(response::GetFileList{std::move(files)});
}

ResPtr makeResGetFileList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
  return Res::fromJson(json);
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

ResPtr makeResPostCommit() {
  return std::make_shared<Res>(response::PostCommit{});
}

ResPtr makeResPostCommit(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to post commit";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::POST_COMMIT);
  return Res::fromJson(json);
}

ResPtr makeResGetAllFileDetails(std::vector<FileDetail> metadata) {
  return std::make_shared<Res>(response::GetAllFileDetails{std::move(metadata)}
  );
}

ResPtr makeResGetAllFileDetails(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get all file details";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_ALL_FILE_DETAILS);
  return Res::fromJson(json);
}

}  // namespace zipfiles