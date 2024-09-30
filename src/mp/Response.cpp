#include <filesystem>
#include <log4cpp/Category.hh>
#include <utility>
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

ResPtr makeResGetFileDetail(FileDetail metadata) {
  return std::make_shared<Res>(response::GetFileDetail{std::move(metadata)});
}

ResPtr makeResGetFileList(std::vector<File> files) {
  return std::make_shared<Res>(response::GetFileList{std::move(files)});
}

// Json::Value json;

// Json::Value newJson;
// newJson["payload"] = json;
// newJson["apiEnum"] = 1;
// fromJson(newJson);

// ResPtr makeResPostCommit(Json::Value json) {
//   json::Value newJson;
//   newJson["payload"] = json;
//   newJson["apiEnum"] = 1;
//   return Res::fromJson(newJson);
// }
ResPtr makeResMockNeedTime(int id) {
  return std::make_shared<Res>(response::MockNeedTime{id});
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
        json["payload"]["name"] = metadata.name;
        json["payload"]["type"] = static_cast<int>(metadata.type);
        json["payload"]["createTime"] = metadata.createTime;
        json["payload"]["updateTime"] = metadata.updateTime;
        json["payload"]["owner"] = metadata.owner;
        json["payload"]["group"] = metadata.group;
        json["payload"]["mode"] = metadata.mode;
        json["payload"]["path"] = metadata.path;
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
        json["apiEnum"] = Json::Value(toSizeT(ApiEnum::IGNORE));
        json["payload"]["id"] = res.id;
      },
      [](auto&&) { throw std::runtime_error("Unknown response type"); }},
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
        json["payload"]["path"].asString(),
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
    case ApiEnum::IGNORE:
      res = makeResMockNeedTime(json["payload"]["id"].asInt());
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

}  // namespace zipfiles