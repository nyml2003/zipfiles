#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <chrono>
#include <thread>
#include "server/socket/socket.h"

namespace zipfiles::server::api {

void mockNeedTime(int client_fd, const Req& req) {
  const auto& request = std::get<request::MockNeedTime>(req.kind);

  response::MockNeedTime response{};
  response.id = request.id;
  // 50ms后再返回
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  Socket::send(client_fd, Res(response, req.uuid, Code::OK));
}

}  // namespace zipfiles::server::api