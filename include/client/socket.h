#ifndef ZIPFILE_CLIENT_SOCKET_H
#define ZIPFILE_CLIENT_SOCKET_H
#include <netinet/in.h>
#include "mp/Request.h"
#include "mp/Response.h"

namespace zipfiles::client {

/**
 * @brief 客户端套接字
 * @details 用于发送请求和接收响应
 */
class Socket {
 public:
  static Socket& getInstance() {
    static Socket instance;
    return instance;
  }
  [[nodiscard]] ResPtr receive() const;
  void send(const ReqPtr& req);
  void reconnect();
  void connectWithRetries();
  void initializeSocket();
  Socket(const Socket& other) = delete;
  Socket& operator=(const Socket& other) = delete;
  Socket(Socket&& other) noexcept = delete;
  Socket& operator=(Socket&& other) noexcept = delete;

 private:
  Socket();
  ~Socket();
  int sock;
  struct sockaddr_in serv_addr;
};
}  // namespace zipfiles::client
#endif  // !ZIPFILE_CLIENT_SOCKET_H
