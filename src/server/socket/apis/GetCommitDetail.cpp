#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <iostream>
#include "json/value.h"
#include "server/restore/restore.h"
#include "server/socket/socket.h"

namespace zipfiles::server::api {

void getCommitDetail(int client_fd, const Req& req) {
  const auto& kind = std::get<request::GetCommitDetail>(req.kind);
  const std::string& uuid = kind.uuid;
  response::GetCommitDetail response;
  Json::Value directoryFile = readDirectoryFileById(uuid);

  for (const auto& file : directoryFile["data"]) {
    fs::path path = file["relativePath"].asString();
    // 拆分成parent_path和filename
    std::string parentPath = path.parent_path().string();
    if (parentPath == "") {
      parentPath = ".";
    }
    std::string name = path.filename().string();

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
  Res res(response, req.uuid, Code::OK);
  Socket::send(client_fd, res);
}

}  // namespace zipfiles::server::api