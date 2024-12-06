#include "mp/Request.h"
#include "mp/Response.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"

namespace zipfiles::server::api {

void CreateNullFolder(int client_fd, const Req& req) {
  auto path = std::get<request::CreateNullFolder>(req.kind).path;
  auto code = Code::OK;
  auto kind = response::CreateNullFolder{};
  auto res = Res{kind, req.uuid, code};
  Socket::send(client_fd, res);
}

}  // namespace zipfiles::server::api