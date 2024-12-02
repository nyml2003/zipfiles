#include "mp/Request.h"
#include "mp/Response.h"
#include "server/configure/configure.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"
#include "server/tools/committable.h"

namespace zipfiles::server::api {

void recoverCommit(int client_fd, const Req& req) {
  Json::Value commitTable = CommitTable::readCommitTableView(COMMIT_TABLE_PATH);
  const auto kind = std::get<request::RecoverCommit>(req.kind);
  CommitTable::recoverCommitRecord(kind.commitId);
  CommitTable::writeCommitTable(COMMIT_TABLE_PATH);
  Socket::send(
    client_fd,
    Res(response::RecoverCommit(), Api::RECOVER_COMMIT, req.uuid, Code::OK)
  );
}

}  // namespace zipfiles::server::api