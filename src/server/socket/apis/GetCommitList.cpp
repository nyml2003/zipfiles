#include <mp/Request.h>
#include <mp/Response.h>
#include <server/backup/backup.h>
#include <server/socket/api.h>
#include <vector>
#include "json/value.h"
#include "server/configure/configure.h"
#include "server/socket/socket.h"

namespace zipfiles::server::api {
void getCommitList(int client_fd, const Req& req) {
  Json::Value commitTable = CommitTable::readCommitTableView(COMMIT_TABLE_PATH);
  response::GetCommitList response;
  for (const auto& uuid : commitTable.getMemberNames()) {
    Json::Value commit = commitTable[uuid];
    if (commit["isDelete"].asBool()) {
      continue;
    }
    response.commits.push_back({
      .uuid = uuid,
      .message = commit["message"].asString(),
      .createTime = commit["createTime"].asDouble(),
      .storagePath = commit["storagePath"].asString(),
      .author = commit["author"].asString(),
      .isEncrypt = commit["isEncrypt"].asBool(),
      .isDelete = commit["isDelete"].asBool(),
    });
  }

  Socket::send(client_fd, Res(response, req.uuid, Code::OK));
}

}  // namespace zipfiles::server::api