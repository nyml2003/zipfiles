#include <log4cpp/Category.hh>
#include <utility>
#include "common.h"
#include "json/value.h"
#include "mp/Response.h"
#include "utils.h"

namespace zipfiles {

Res::Res(ResKind kind) : kind(std::move(kind)), timestamp(0) {}

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
size_t toSizeT(ApiEnum apiEnum) {
  return static_cast<size_t>(apiEnum);
}
Json::Value Res::toJson() {
  Json::Value json;
  Json::Value payload;
  Json::Value kindValue;
  payload = Json::objectValue;
  std::visit(
    overload{
      [&payload, &kindValue](response::GetFileDetail& res) {
        kindValue = Json::Value(toSizeT(ApiEnum::GET_FILE_DETAIL));
        const auto& metadata = res.metadata;
        payload["name"] = metadata.name;
        payload["type"] = toDouble(metadata.type);
        payload["createTime"] = metadata.createTime;
        payload["updateTime"] = metadata.updateTime;
        payload["owner"] = metadata.owner;
        payload["group"] = metadata.group;
        payload["mode"] = metadata.mode;
        payload["path"] = metadata.path;
        payload["size"] = static_cast<Json::UInt64>(metadata.size);
      },
      [&payload, &kindValue](response::GetFileList& res) {
        kindValue = Json::Value(toSizeT(ApiEnum::GET_FILE_LIST));
        payload["files"] = Json::arrayValue;
        for (const auto& file : res.files) {
          Json::Value fileJson;
          fileJson["name"] = file.name;
          fileJson["type"] = toDouble(file.type);
          payload["files"].append(fileJson);
        }
      },
      [](auto&&) { throw std::runtime_error("Unknown response type"); }},
    kind
  );
  json["kind"] = kindValue;
  json["payload"] = payload;
  json["status"] = static_cast<int>(status);
  json["timestamp"] = timestamp;
  return json;
}

ResPtr Res::fromJson(const Json::Value& json) {
  ResPtr res;
  auto api = static_cast<ApiEnum>(json["kind"].asInt());
  switch (api) {
    case ApiEnum::GET_FILE_DETAIL: {
      FileDetail metadata = {
        toFileType(json["payload"]["type"].asDouble()),
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
          {file["name"].asString(), toFileType(file["type"].asDouble())}
        );
      }
      res = makeResGetFileList(files);
      break;
    }
    default:
      throw std::runtime_error("Invalid response kind");
      break;
  }
  res->status = static_cast<StatusCode>(json["status"].asInt());
  res->timestamp = json["timestamp"].asDouble();
  return res;
}

}  // namespace zipfiles