#ifndef ZIPFILE_CLIENT_SOCKET_H
#define ZIPFILE_CLIENT_SOCKET_H
#include <json/json.h>
#include <netinet/in.h>
#include <atomic>
#include <functional>

namespace zipfiles::client {
/**
 * @brief 解析状态(前端socket兼顾解析响应的功能)
 *
 */
enum class ReceiveStatus { READ_DATA_SIZE, READ_DATA };

/**
 * @brief socket状态
 *
 */
enum class SocketStatus { CONNECT, DISCONNECTED };

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
  void receive(const std::function<void(const Json::Value&)>& callback);
  void send(const std::string& req);
  void connectWithRetries();
  void initializeSocket();
  void disconnect();
  Socket(const Socket& other) = delete;
  Socket& operator=(const Socket& other) = delete;
  Socket(Socket&& other) noexcept = delete;
  Socket& operator=(Socket&& other) noexcept = delete;
  // 是否应该继续接收数据
  std::atomic_bool active;
  // 恢复socket的触发器
  std::atomic_bool trigger;
  // 是否disconnet
  std::atomic_bool disconnectFlag;

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
  SocketStatus socketStatus{};

  void readDataSize(uint8_t byte);
  bool readData(uint8_t byte);
  Json::Value parseJsonFromBuffer();
};
}  // namespace zipfiles::client
#endif  // !ZIPFILE_CLIENT_SOCKET_H
