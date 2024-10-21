#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <filesystem>
#include "server/backup/backup.h"
#include "server/tools/committable.h"

// namespace zipfiles::server::api {

template <>
response::PostCommit handle<request::PostCommit, response::PostCommit>(
  const request::PostCommit& request
) {
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

  backupFiles(files, cr, request.isEncrypt ? request.key.value() : "");

  return {};
}

// }  // namespace zipfiles::server::api