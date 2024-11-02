#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <fstream>
#include <iostream>
#include "json/value.h"
#include "server/restore/restore.h"

namespace zipfiles::server::api {

template <>
response::GetCommitDetail handle(const request::GetCommitDetail& request) {
  const std::string& uuid = request.uuid;
  //
  // std::ifstream inFile(path);
  // if (!inFile) {
  //   throw std::runtime_error("Failed to open file: " + path.string());
  // }
  response::GetCommitDetail response;
  // Json::Value directoryFile;
  // inFile >> directoryFile;
  // inFile.close();
  Json::Value directoryFile = readDirectoryFileById(uuid);
  std::cout << directoryFile;

  for (const auto& file : directoryFile["data"]) {
    fs::path path = file["relativePath"].asString();
    std::cout << path << std::endl;
    // 拆分成parent_path和filename
    std::string parentPath = path.parent_path().string();
    std::string name = path.filename().string();
    std::cout << "parentPath: " << parentPath << std::endl;
    std::cout << "name: " << name << std::endl;

    response.files.push_back(
      {.type = static_cast<fs::file_type>(file["type"].asInt()),
       .createTime = file["createTime"].asDouble(),
       .updateTime = file["updateTime"].asDouble(),
       .size = file["size"].asInt64(),
       .owner = file["owner"].asString(),
       .group = file["group"].asString(),
       .mode = static_cast<mode_t>(file["mode"].asInt()),
       .path = parentPath,
       .name = name}
    );
  }
  return response;
}

}  // namespace zipfiles::server::api