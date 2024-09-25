#include "mp/GetFileDetail.h"
#include "utils.h"

namespace zipfiles::mp {

Json::Value GetFileDetailRequest::toJson() {
  Json::Value json;
  json["path"] = path;
  return json;
}

void GetFileDetailRequest::fromJson(const Json::Value& json) {
  path = json["path"].asString();
}

void GetFileDetailRequest::setPath(const std::string& path) {
  this->path = path;
}

std::string GetFileDetailRequest::getPath() {
  return path;
}

Json::Value GetFileDetailResponse::toJson() {
  Json::Value json;
  json["metadata"] = Json::Value(Json::objectValue);
  json["metadata"]["name"] = metadata.name;
  json["metadata"]["type"] = zipfiles::toString(metadata.type);
  json["metadata"]["createTime"] = metadata.createTime;
  json["metadata"]["updateTime"] = metadata.updateTime;
  json["metadata"]["owner"] = metadata.owner;
  json["metadata"]["group"] = metadata.group;
  json["metadata"]["mode"] = metadata.mode;
  json["metadata"]["path"] = metadata.path;
  return json;
}

void GetFileDetailResponse::fromJson(const Json::Value& json) {
  metadata.name = json["metadata"]["name"].asString();
  metadata.type = zipfiles::toFileType(json["metadata"]["type"].asString());
  metadata.createTime =
    toIso8601(toTm(json["metadata"]["updateTime"].asUInt64()));
  metadata.updateTime =
    toIso8601(toTm(json["metadata"]["updateTime"].asUInt64()));
  metadata.size = json["metadata"]["size"].asUInt64();
  metadata.owner = json["metadata"]["owner"].asString();
  metadata.group = json["metadata"]["group"].asString();
  metadata.mode = json["metadata"]["mode"].asUInt();
  metadata.path = json["metadata"]["path"].asString();
}

void GetFileDetailResponse::setMetadata(const FileDetail& metadata) {
  this->metadata = metadata;
}

FileDetail GetFileDetailResponse::getMetadata() {
  return metadata;
}

}  // namespace zipfiles::mp