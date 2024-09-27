#include "mp/Response.h"

#include <utility>
#include "common.h"
#include "json/value.h"
#include "utils.h"

namespace zipfiles {

Res::Res(ResKind kind) : kind(std::move(kind)) {}

ResPtr makeResGetFileDetail(FileDetail metadata) {
  return std::make_shared<Res>(response::GetFileDetail{std::move(metadata)});
}

ResPtr makeResGetFileList(std::vector<File> files) {
  return std::make_shared<Res>(response::GetFileList{std::move(files)});
}

Json::Value Res::toJson() {
  Json::Value json;
  json["kind"] = kind.index();
  Json::Value payload;  // 将 payload 声明移到外部
  std::visit(
    overload{
      [&payload](const response::GetFileDetail& res) {
        const auto& metadata = res.metadata;
        payload = Json::objectValue;
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
      [&payload](const response::GetFileList& res) {
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
  json["payload"] = payload;  // 将 payload 赋值给 json["payload"]
  return json;
}

ResPtr Res::fromJson(const Json::Value& json) {
  ResPtr res;
  switch (json["kind"].asInt()) {
    case 0: {
      FileDetail metadata = {
        toFileType(json["payload"]["type"].asDouble()),
        json["payload"]["createTime"].asString(),
        json["payload"]["updateTime"].asString(),
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
    case 1: {
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
  return res;
}

}  // namespace zipfiles