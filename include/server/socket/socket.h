#ifndef ZIPFILE_SERVER_SOCKET_H
#define ZIPFILE_SERVER_SOCKET_H
#include "mp/mp.h"
namespace zipfiles::server {
/**
 * @brief 服务器套接字
 * @details 用于接收请求和发送响应
 */
class ServerSocket {
 public:
  static ServerSocket& getInstance() {
    static ServerSocket instance;
    return instance;
  }
  [[nodiscard]] static bool receive(const mp::RequestPtr& req);
  [[nodiscard]] static bool send(const mp::ResponsePtr& res);

  static void acceptConnection();
  ServerSocket(const ServerSocket& other) = delete;
  ServerSocket& operator=(const ServerSocket& other) = delete;
  ServerSocket(ServerSocket&& other) noexcept = delete;
  ServerSocket& operator=(ServerSocket&& other) noexcept = delete;
  [[nodiscard]] static int getServerFd();

 private:
  ServerSocket();
  ~ServerSocket();
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen;
};
}  // namespace zipfiles::server
#endif  // !ZIPFILE_SERVER_SOCKET_SOCKET_H