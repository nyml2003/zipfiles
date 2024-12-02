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
  Res res(response, Api::RESTORE, req.uuid, Code::OK);
  Socket::send(client_fd, res);
  Json::Value idJson;
  idJson["id"] = messageId;
  try {
    restoreTo(path, commitId, key);
    Socket::send(
      client_fd, Notification(
                   std::string("restore success"), Code::RESTORE_SUCCESS, idJson
                 )
    );
  } catch (const std::exception& e) {
    Socket::send(
      client_fd,
      Notification(
        std::string("restore failed: ") + e.what(), Code::RESTORE_FAILED, idJson
      )
    );
    return;
  }
}

}  // namespace zipfiles::server::api