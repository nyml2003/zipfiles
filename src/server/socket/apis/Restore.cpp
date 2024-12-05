#include "server/restore/restore.h"
#include "mp/Request.h"
#include "mp/Response.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"

namespace zipfiles::server::api {

void restore(int client_fd, const Req& req) {
  const auto& kind = std::get<request::Restore>(req.kind);
  const auto key = kind.key.value_or("");
  const std::string& commitId = kind.commitId;
  const std::string& messageId = kind.messageId;
  const std::string& path = kind.path;
  response::Restore response;
  Res res(response, req.uuid, Code::OK);
  Socket::send(client_fd, res);
  try {
    restoreTo(path, commitId, key);
    Socket::send(
      client_fd, Notification(
                   notification::Restore{
                     .messageId = messageId, .description = "还原成功"
                   },
                   Code::RESTORE_SUCCESS
                 )
    );
  } catch (const std::exception& e) {
    Socket::send(
      client_fd,
      Notification(
        notification::Restore{.messageId = messageId, .description = e.what()},
        Code::RESTORE_FAILED
      )
    );
    return;
  }
}

}  // namespace zipfiles::server::api