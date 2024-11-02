#ifndef ZIPFILE_CLIENT_SOCKET_H
#define ZIPFILE_CLIENT_SOCKET_H
#include <netinet/in.h>
#include "mp/Request.h"
#include "mp/Response.h"

namespace zipfiles::client {
/**
 * @brief 解析状态(前端socket兼顾解析响应的功能)
 *
 */
enum class ReceiveStatus { READ_DATA_SIZE, READ_DATA };

/**
 * @brief 客户端套接字
 *
 * @details 用于发送请求和接收响应
 *
 */
class Socket {
 public:
  static Socket& getInstance() {
    static Socket instance;
    return instance;
  }
  void receive();
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
  int server_fd;
  struct sockaddr_in serv_addr;
  std::vector<uint8_t> header_buffer;
  std::vector<uint8_t> read_buffer;
  std::vector<uint8_t> write_buffer;
  uint32_t data_size{};
  ReceiveStatus state{};

  void readDataSize(uint8_t byte);
  bool readData(uint8_t byte);
  Json::Value parseJsonFromBuffer();
};
}  // namespace zipfiles::client
#endif  // !ZIPFILE_CLIENT_SOCKET_H
