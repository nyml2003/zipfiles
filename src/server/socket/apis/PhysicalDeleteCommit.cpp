#include "mp/Request.h"
#include "mp/Response.h"
#include "server/backup/backup.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"

namespace zipfiles::server::api {

void physicalDeleteCommit(int client_fd, const Req& req) {
  auto kind = std::get<request::PhysicalDeleteCommit>(req.kind);

  removeCommitAndRecordById(kind.commitId);
  Socket::send(
    client_fd, Res(
                 response::PhysicalDeleteCommit(), Api::PHYSICAL_DELETE_COMMIT,
                 req.uuid, Code::OK
               )
  );
}

}  // namespace zipfiles::server::api