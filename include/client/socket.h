#ifndef ZIPFILE_CLIENT_SOCKET_H
#define ZIPFILE_CLIENT_SOCKET_H
#include "mp/mp.h"
namespace zipfiles::client::socket {
/**
 * @brief 客户端套接字
 * @details 用于发送请求和接收响应
 */
class ClientSocket {
 public:
  static ClientSocket& getInstance() {
    static ClientSocket instance;
    return instance;
  }
  [[nodiscard]] static bool receive(const mp::ResponsePtr& res);
  [[nodiscard]] static bool send(const mp::RequestPtr& req);
  ClientSocket(const ClientSocket& other) = delete;
  ClientSocket& operator=(const ClientSocket& other) = delete;
  ClientSocket(ClientSocket&& other) noexcept = delete;
  ClientSocket& operator=(ClientSocket&& other) noexcept = delete;

 private:
  ClientSocket();
  ~ClientSocket();
  int sock;
  struct sockaddr_in serv_addr;
};
}  // namespace zipfiles::client::socket
#endif  // !ZIPFILE_CLIENT_SOCKET_H
