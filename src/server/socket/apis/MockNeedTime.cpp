#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include "server/socket/socket.h"

namespace zipfiles::server::api {

void mockNeedTime(int client_fd, const Req& req) {
  const auto& request = std::get<request::MockNeedTime>(req.kind);

  response::MockNeedTime response{};
  response.id = request.id;
  Socket::send(client_fd, Res(response, req.uuid, Code::OK));
}

}  // namespace zipfiles::server::api