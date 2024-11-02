#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <fstream>
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

  for (const auto& file : directoryFile["data"]) {
    response.files.push_back(
      {.type = static_cast<fs::file_type>(file["type"].asInt()),
       .createTime = file["createTime"].asDouble(),
       .updateTime = file["updateTime"].asDouble(),
       .size = file["size"].asInt64(),
       .owner = file["owner"].asString(),
       .group = file["group"].asString(),
       .mode = static_cast<mode_t>(file["mode"].asInt()),
       .path = file["relativePath"].asString(),
       .name = fs::path(file["relativePath"].asString()).filename().string()}
    );
  }
  return response;
}

}  // namespace zipfiles::server::api