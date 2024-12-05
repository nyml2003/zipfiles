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
  Socket::send(client_fd, Res(response::PostCommit{}, req.uuid, Code::OK));
  try {
    backupFiles(files, cr, request.isEncrypt ? request.key.value() : "");
    Socket::send(
      client_fd, Notification(
                   notification::Restore{
                     .messageId = request.uuid, .description = "备份成功"
                   },
                   Code::RESTORE_SUCCESS
                 )
    );
  } catch (const std::exception& e) {
    Socket::send(
      client_fd, Notification(
                   notification::Restore{
                     .messageId = request.uuid, .description = e.what()
                   },
                   Code::RESTORE_FAILED
                 )
    );
  }
}

}  // namespace zipfiles::server::api