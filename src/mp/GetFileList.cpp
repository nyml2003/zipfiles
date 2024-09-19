#include "mp/GetFileList.h"

namespace zipfiles::mp {

Json::Value GetFileListRequest::toJson() {
  Json::Value json;
  json["path"] = path;
  return json;
}

void GetFileListRequest::fromJson(const Json::Value& json) {
  path = json["path"].asString();
}

void GetFileListRequest::setPath(std::string path) {
  this->path = std::move(path);
}

std::string GetFileListRequest::getPath() {
  return path;
}

Json::Value GetFileListResponse::toJson() {
  Json::Value json;
  json["files"] = Json::Value(Json::arrayValue);
  for (const auto& file : files) {
    Json::Value file_json;
    file_json["name"] = file.name;
    file_json["type"] = file.type == FileType::FILE ? "file" : "directory";
    json["files"].append(file_json);
  }
  return json;
}

void GetFileListResponse::fromJson(const Json::Value& json) {
  files.clear();
  for (const auto& file_json : json["files"]) {
    File file;
    file.name = file_json["name"].asString();
    file.type = file_json["type"].asString() == "file" ? FileType::FILE
                                                       : FileType::DIRECTORY;
    files.push_back(file);
  }
}

void GetFileListResponse::setFiles(const std::vector<File>& files) {
  for (const auto& file : files) {
    this->files.push_back(file);
  }
}

std::vector<File> GetFileListResponse::getFiles() {
  return files;
}

}  // namespace zipfiles::mp