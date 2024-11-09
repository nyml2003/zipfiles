#include <filesystem>

#include "mp/Request.h"
#include "mp/Response.h"
#include "server/backup/backup.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"
#include "server/tools/committable.h"

namespace zipfiles::server::api {

void postCommit(int client_fd, const Req& req) {
  const auto& request = std::get<request::PostCommit>(req.kind);
  const std::vector<fs::path> files(request.files.begin(), request.files.end());
  CommitTableRecord cr = {
    .uuid = request.uuid,
    .message = request.message,
    .createTime = request.createTime,
    .storagePath = request.storagePath,
    .author = request.author,
    .isEncrypt = request.isEncrypt,
    .isDelete = false,
  };
  Socket::send(
    client_fd, Res(response::PostCommit{}, Api::POST_COMMIT, req.uuid, Code::OK)
  );
  Json::Value idJson;
  idJson["id"] = request.uuid;
  try {
    backupFiles(files, cr, request.isEncrypt ? request.key.value() : "");
    Socket::send(
      client_fd,
      Notification(
        std::string("commit success"), Code::POSTCOMMIT_SUCCESS, idJson
      )
    );
  } catch (const std::exception& e) {
    Socket::send(
      client_fd, Notification(
                   std::string("commit failed: ") + e.what(),
                   Code::POSTCOMMIT_FAILED, idJson
                 )
    );
  }
}

}  // namespace zipfiles::server::api