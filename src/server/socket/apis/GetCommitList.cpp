#include <mp/Request.h>
#include <mp/Response.h>
#include <server/backup/backup.h>
#include <server/socket/api.h>

namespace zipfiles::server::api {

template <>
response::GetCommitList handle<request::GetCommitList, response::GetCommitList>(
  const request::GetCommitList& request
) {
  response::GetCommitList response;

  return response;
}

}  // namespace zipfiles::server::api