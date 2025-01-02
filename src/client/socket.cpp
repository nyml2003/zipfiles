#include "client/socket.h"

#include "client/launcher.h"
#include "client/view.h"

#include "mp/Notification.h"
#include "mp/common.h"

#include <arpa/inet.h>
#include <cstdint>
#include <log4cpp/Category.hh>
#include <thread>
#include <vector>

namespace zipfiles::client {
/**
 * @brief 初始化socket
 */
void Socket::initializeSocket() {
  log4cpp::Category::getRoot().infoStream() << "Initializing socket...";
  server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    log4cpp::Category::getRoot().errorStream() << "Socket creation error";
    socketStatus = SocketStatus::DISCONNECTED;
    throw std::runtime_error("Socket creation error");
    return;
  }

  log4cpp::Category::getRoot().infoStream() << "Socket created";

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  log4cpp::Category::getRoot().infoStream() << "Port set to " << PORT;
  log4cpp::Category::getRoot().infoStream() << "Resolving address...";

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) != 1) {
    log4cpp::Category::getRoot().errorStream()
      << "Invalid address/ Address not supported";

    close(server_fd);
    socketStatus = SocketStatus::DISCONNECTED;
    throw std::runtime_error("Invalid address/ Address not supported");
    return;
  }
}

/**
 * @brief 创建一个client socket对象，并且尝试连接server
 *
 */
Socket::Socket()
  : active(false),
    trigger(false),
    disconnectFlag(false),
    server_fd(-1),
    serv_addr{},
    socketStatus(SocketStatus::DISCONNECTED) {}

/**
 * @brief 尝试连接server，具有重试机制
 *
 */
void Socket::connectWithRetries() {
  if (disconnectFlag) {
    initializeSocket();
    disconnectFlag = false;
  }
  const int max_retries = 5;
  int retries = 0;
  while (retries < max_retries) {
    if (connect(server_fd, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
      if (!Launcher::getInstance().isRunning) {
        log4cpp::Category::getRoot().errorStream()
          << "Application is not running";
        return;
      }
      log4cpp::Category::getRoot().errorStream() << "Connection failed";
      retries++;
      log4cpp::Category::getRoot().errorStream()
        << "Retrying connection (" << retries << "/" << max_retries << ")...";
      handleNotify(ZNotification(
        notification::DoubleLine(
          {.title = "正在重试连接",
           .description = "第" + std::to_string(retries) + "次尝试"}
        ),
        Code::DOUBLE_WARNING
      ));
      handleNotify(
        ZNotification(notification::NoneNotification(), Code::SOCKET_CONNECT)
      );
      std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待1秒后重试
    } else {
      socketStatus = SocketStatus::CONNECT;
      active = true;
      handleNotify(ZNotification(
        notification::NoneNotification(), Code::ENABLE_REMOTE_REQUEST
      ));
      handleNotify(ZNotification(
        notification::DoubleLine(
          {.title = "连接成功", .description = "与服务器的连接已建立"}
        ),
        Code::DOUBLE_SUCCESS
      ));
      return;
    }
  }
  log4cpp::Category::getRoot().errorStream()
    << "Failed to connect to the server after " << max_retries << " attempts.";
  disconnect();
}

Socket::~Socket() {
  close(server_fd);
}

/**
 * @brief 向server发送序列化后的请求
 *
 * @param req 请求对象
 *
 */
void Socket::send(const std::string& req) {
  if (socketStatus == SocketStatus::DISCONNECTED) {
    throw std::runtime_error("服务端未连接");
  }
  uint32_t dataSize = req.size();
  std::string data =
    std::string(reinterpret_cast<char*>(&dataSize), sizeof(dataSize)) + req;

  ssize_t bytesSent = ::send(server_fd, data.c_str(), data.size(), 0);

  if (bytesSent == -1) {
    handleNotify(ZNotification(
      notification::DoubleLine(
        {.title = "发送请求失败",
         .description = "无法发送请求" + std::to_string(errno)}
      ),
      Code::DOUBLE_ERROR
    ));
    disconnect();
    return;
  }
}

void Socket::disconnect() {
  close(server_fd);
  active = false;
  disconnectFlag = true;
  socketStatus = SocketStatus::DISCONNECTED;
  handleNotify(ZNotification(
    notification::NoneNotification(), Code::DISABLE_REMOTE_REQUEST
  ));
}

/**
 * @brief 从server接收并解析response
 *
 * @return ResPtr 解析出的response
 *
 */
void Socket::receive(const std::function<void(const Json::Value&)>& callback) {
  if (socketStatus == SocketStatus::DISCONNECTED) {
    connectWithRetries();
    if (socketStatus == SocketStatus::DISCONNECTED) {
      trigger.store(false);
      return;
    }
  }
  read_buffer.resize(MAX_MESSAGE_SIZE);  // 预留空间

  ssize_t bytesRead = read(server_fd, read_buffer.data(), MAX_MESSAGE_SIZE);

  if (bytesRead > 0) {
    read_buffer.resize(bytesRead);  // 调整缓冲区大小以适应实际读取的数据
  }

  // 连接是否关闭
  if (bytesRead == 0) {
    disconnect();
    handleNotify(ZNotification(
      notification::DoubleLine(
        {.title = "连接断开", .description = "与服务器的连接已断开"}
      ),
      Code::DOUBLE_ERROR
    ));
    return;
  }

  // socket是否产生了错误
  if (bytesRead < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // 非阻塞模式下没有数据可读，不做任何处理
      return;
    }
    disconnect();
    handleNotify(ZNotification(
      notification::DoubleLine(
        {.title = "接收响应失败",
         .description = "无法接收响应" + std::to_string(errno)}
      ),
      Code::DOUBLE_ERROR
    ));
    return;
  }
  // read_buffer仍然有内容
  for (const uint8_t byte : read_buffer) {
    switch (state) {
      case ReceiveStatus::READ_DATA_SIZE:
        readDataSize(byte);
        break;
      case ReceiveStatus::READ_DATA:
        if (readData(byte)) {
          callback(parseJsonFromBuffer());
          state = ReceiveStatus::READ_DATA_SIZE;
        }
        break;
      default:
        throw std::runtime_error("Unknown state");
    }
  }

  read_buffer.clear();
}

/**
 * @brief 尝试从读取的数据中读取出Json长度
 *
 * @param byte 读取的数据的一个字节
 *
 */
inline void Socket::readDataSize(uint8_t byte) {
  header_buffer.push_back(byte);

  if (header_buffer.size() == 4) {
    data_size = *reinterpret_cast<uint32_t*>(header_buffer.data());

    if (data_size == 0) {
      throw std::runtime_error("Illegal data_size, it is zero");
    }

    header_buffer.clear();
    state = ReceiveStatus::READ_DATA;
  } else if (header_buffer.size() > 4) {
    throw std::runtime_error("Illegal header buffer size");
  }
}

/**
 * @brief 尝试从读取的数据中读取出数据内容
 *
 * @param byte 读取的数据的一个字节
 *
 */
inline bool Socket::readData(uint8_t byte) {
  write_buffer.push_back(byte);

  return data_size == write_buffer.size();
}

/**
 * @brief 尝试从读取出的数据内容中解析出一个Json
 *
 */
Json::Value Socket::parseJsonFromBuffer() {
  Json::Reader reader;
  Json::Value jsonData;
  std::string jsonString(write_buffer.begin(), write_buffer.end());

  if (reader.parse(jsonString, jsonData)) {
    write_buffer.clear();
    log4cpp::Category::getRoot().infoStream()
      << "Received response: " << jsonData;
    return jsonData;
  }
  write_buffer.clear();
  throw std::runtime_error("Illegal json format");
}

}  // namespace zipfiles::client