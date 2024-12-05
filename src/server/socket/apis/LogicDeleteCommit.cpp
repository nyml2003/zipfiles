#include "mp/Request.h"
#include "mp/Response.h"
#include "server/configure/configure.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"
#include "server/tools/committable.h"

namespace zipfiles::server::api {

void logicDeleteCommit(int client_fd, const Req& req) {
  Json::Value commitTable = CommitTable::readCommitTableView(COMMIT_TABLE_PATH);
  const auto kind = std::get<request::LogicDeleteCommit>(req.kind);
  CommitTable::deleteCommitRecord(kind.commitId);
  CommitTable::writeCommitTable(COMMIT_TABLE_PATH);
  Socket::send(
    client_fd, Res(response::LogicDeleteCommit(), req.uuid, Code::OK)
  );
}

}  // namespace zipfiles::server::api