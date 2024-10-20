#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>

namespace zipfiles::server::api {

template <>
response::PostCommit handle<request::PostCommit, response::PostCommit>(
  request::PostCommit request
) {
  response::PostCommit response;
  return response;
}

}  // namespace zipfiles::server::api