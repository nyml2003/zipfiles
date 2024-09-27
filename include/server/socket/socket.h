#ifndef ZIPFILE_SERVER_SOCKET_H
#define ZIPFILE_SERVER_SOCKET_H
#include "mp/Request.h"
#include "mp/Response.h"
#include <netinet/in.h>
namespace zipfiles::server {
/**
 * @brief 服务器套接字
 * @details 用于接收请求和发送响应
 */
class Socket {
 public:
  static Socket& getInstance() {
    static Socket instance;
    return instance;
  }
  [[nodiscard]] static ReqPtr receive();
  static void send(const ResPtr& res);

  static void acceptConnection();
  Socket(const Socket& other) = delete;
  Socket& operator=(const Socket& other) = delete;
  Socket(Socket&& other) noexcept = delete;
  Socket& operator=(Socket&& other) noexcept = delete;
  [[nodiscard]] static int getServerFd();

 private:
  Socket();
  ~Socket();
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen;
};
}  // namespace zipfiles::server
#endif  // !ZIPFILE_SERVER_SOCKET_SOCKET_H