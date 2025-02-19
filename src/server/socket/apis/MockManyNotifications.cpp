#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <server/socket/socket.h>
#include <thread>
#include "mp/Notification.h"

namespace zipfiles::server::api {

void mockManyNotifications(int client_fd, const Req& req) {
  response::MockManyNotifications response;
  response.message = "耗时操作已收到，正在处理中";
  Socket::send(client_fd, Res(response, req.uuid, Code::OK));
  std::thread([client_fd, req] {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::string message = "耗时操作已完成";
    Socket::send(
      client_fd, Notification(
                   notification::SingleLine{.title = message}, Code::SINGLE_INFO
                 )
    );
  }).detach();
}

}  // namespace zipfiles::server::api