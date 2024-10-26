#ifndef ZIPFILE_SERVER_SOCKET_H
#define ZIPFILE_SERVER_SOCKET_H

#include <netinet/in.h>
#include <atomic>
#include <mutex>
#include "mp/Request.h"
#include "mp/Response.h"

/**
 * @brief 建立的最大连接数量(最大client数量)
 *
 */
#define MAX_CONNECTIONS 3

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
  [[nodiscard]] static ReqPtr receive(int client_fd);
  static void send(int client_fd, const ResPtr& res);

  static void acceptConnection(int epoll_fd);
  Socket(const Socket& other) = delete;
  Socket& operator=(const Socket& other) = delete;
  Socket(Socket&& other) noexcept = delete;
  Socket& operator=(Socket&& other) noexcept = delete;
  [[nodiscard]] static int getServerFd();

 private:
  Socket();
  ~Socket();
  int server_fd;
  struct sockaddr_in address;
  int addrlen;
};
}  // namespace zipfiles::server
#endif  // !ZIPFILE_SERVER_SOCKET_SOCKET_H