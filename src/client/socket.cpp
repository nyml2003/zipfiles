#include "client/socket.h"

#include "client/view.h"
#include "log4cpp/Category.hh"
#include "mp/Notification.h"
#include "mp/common.h"

#include <arpa/inet.h>
#include <cstdint>
#include <thread>
#include <vector>

namespace zipfiles::client {
/**
 * @brief 初始化socket
 */
void Socket::initializeSocket() {
  log4cpp::Category::getRoot().infoStream() << "Initializing socket...";
  handleNotify(ZNotification(
    notification::DoubleLine(
      {.title = "正在建立连接", .description = "正在初始化socket"}
    ),
    Code::DOUBLE_INFO
  ));
  server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    log4cpp::Category::getRoot().errorStream() << "Socket creation error";
    handleNotify(ZNotification(
      notification::DoubleLine(
        {.title = "建立连接失败", .description = "Socket创建失败"}
      ),
      Code::DOUBLE_ERROR
    ));
    socketStatus = SocketStatus::DISCONNECTED;
    return;
  }

  log4cpp::Category::getRoot().infoStream() << "Socket created";

  handleNotify(ZNotification(
    notification::DoubleLine(
      {.title = "建立连接成功", .description = "Socket创建成功"}
    ),
    Code::DOUBLE_INFO
  ));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  log4cpp::Category::getRoot().infoStream() << "Port set to " << PORT;
  log4cpp::Category::getRoot().infoStream() << "Resolving address...";

  handleNotify(ZNotification(
    notification::DoubleLine(
      {.title = "正在解析地址", .description = "正在解析地址"}
    ),
    Code::DOUBLE_INFO
  ));
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) != 1) {
    log4cpp::Category::getRoot().errorStream()
      << "Invalid address/ Address not supported";

    close(server_fd);
    handleNotify(ZNotification(
      notification::DoubleLine(
        {.title = "建立连接失败", .description = "无效地址/不支持的地址"}
      ),
      Code::DOUBLE_ERROR
    ));
    socketStatus = SocketStatus::DISCONNECTED;
    return;
  }
  socketStatus = SocketStatus::PENDING;
}

/**
 * @brief 创建一个client socket对象，并且尝试连接server
 *
 */
Socket::Socket()
  : server_fd(-1),
    serv_addr{},
    socketStatus(SocketStatus::DISCONNECTED),
    active(true) {
  try {
    initializeSocket();
    connectWithRetries();
  } catch (const std::exception& e) {
    close(server_fd);
    throw e;
  }
}

/**
 * @brief 尝试连接server，具有重试机制
 *
 */
void Socket::connectWithRetries() {
  if (socketStatus != SocketStatus::PENDING) {
    return;
  }
  const int max_retries = 5;
  int retries = 0;
  while (retries < max_retries) {
    if (connect(
          server_fd, reinterpret_cast<struct sockaddr*>(&serv_addr),
          sizeof(serv_addr)
        ) < 0) {
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
      std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待1秒后重试
    } else {
      socketStatus = SocketStatus::CONNECT;
      return;
    }
  }

  log4cpp::Category::getRoot().errorStream()
    << "Failed to connect to the server after " << max_retries << " attempts.";
  close(server_fd);
  handleNotify(ZNotification(
    notification::DoubleLine(
      {.title = "建立连接失败", .description = "无法连接到服务器"}
    ),
    Code::DOUBLE_ERROR
  ));
  active = false;
  socketStatus = SocketStatus::DISCONNECTED;
}

Socket::~Socket() {
  close(server_fd);
}

/**
 * @brief 尝试重新连接server
 *
 */
void Socket::reconnect() {
  close(server_fd);
  try {
    initializeSocket();
    connectWithRetries();
  } catch (const std::exception& e) {
    close(server_fd);
    throw e;
  }
}

/**
 * @brief 向server发送序列化后的请求
 *
 * @param req 请求对象
 *
 */
void Socket::send(const std::string& req) {
  if (socketStatus == SocketStatus::DISCONNECTED) {
    reconnect();
    if (socketStatus == SocketStatus::DISCONNECTED) {
      return;
    }
  }
  uint32_t dataSize = req.size();
  std::string data =
    std::string(reinterpret_cast<char*>(&dataSize), sizeof(dataSize)) + req;

  ssize_t bytesSent = ::send(server_fd, data.c_str(), data.size(), 0);

  if (bytesSent == -1) {
    if (errno == EPIPE) {  // 检测到 "Broken pipe" 错误
      log4cpp::Category::getRoot().errorStream()
        << "Failed to send request: Broken pipe. Attempting to reconnect...";

      close(server_fd);
      reconnect();  // 尝试重新连接
      bytesSent =
        ::send(server_fd, data.c_str(), data.size(), 0);  // 重新发送请求
      if (bytesSent == -1) {
        handleNotify(ZNotification(
          notification::DoubleLine(
            {.title = "发送请求失败",
             .description = "无法发送请求" + std::to_string(errno)}
          ),
          Code::DOUBLE_ERROR
        ));
        return;
      }

    } else {
      handleNotify(ZNotification(
        notification::DoubleLine(
          {.title = "发送请求失败",
           .description = "无法发送请求" + std::to_string(errno)}
        ),
        Code::DOUBLE_ERROR
      ));
      return;
    }
  }
}

/**
 * @brief 从server接收并解析response
 *
 * @return ResPtr 解析出的response
 *
 */
void Socket::receive(const std::function<void(const Json::Value&)>& callback) {
  if (socketStatus == SocketStatus::DISCONNECTED) {
    reconnect();
  }
  if (socketStatus == SocketStatus::PENDING) {
    handleNotify(ZNotification(
      notification::DoubleLine(
        {.title = "接收响应失败", .description = "连接尚未建立"}
      ),
      Code::DOUBLE_ERROR
    ));
    return;
  }
  read_buffer.resize(MAX_MESSAGE_SIZE);  // 预留空间

  ssize_t bytesRead = read(server_fd, read_buffer.data(), MAX_MESSAGE_SIZE);

  if (bytesRead > 0) {
    read_buffer.resize(bytesRead);  // 调整缓冲区大小以适应实际读取的数据
  }

  // 连接是否关闭
  if (bytesRead == 0) {
    // 断开连接
    close(server_fd);
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
    close(server_fd);
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      handleNotify(ZNotification(
        notification::DoubleLine(
          {.title = "连接超时", .description = "连接超时"}
        ),
        Code::DOUBLE_ERROR
      ));
      return;
    }
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