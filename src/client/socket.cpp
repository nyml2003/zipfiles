#include "client/socket.h"

#include "client/view.h"
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/mp.h"

#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <thread>
#include <vector>

namespace zipfiles::client {
/**
 * @brief 初始化socket
 *
 */
void Socket::initializeSocket() {
  log4cpp::Category::getRoot().infoStream() << "Initializing socket...";
  server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    log4cpp::Category::getRoot().errorStream() << "Socket creation error";

    throw std::runtime_error("Socket creation error");
  }

  log4cpp::Category::getRoot().infoStream() << "Socket created";

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(mp::PORT);

  log4cpp::Category::getRoot().infoStream() << "Port set to " << mp::PORT;
  log4cpp::Category::getRoot().infoStream() << "Resolving address...";

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Invalid address/ Address not supported";

    close(server_fd);

    throw std::runtime_error("Invalid address/ Address not supported");
  }
}

/**
 * @brief 创建一个client socket对象，并且尝试连接server
 *
 */
Socket::Socket() : server_fd(-1), serv_addr{} {
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
      std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待1秒后重试
    } else {
      return;
    }
  }

  log4cpp::Category::getRoot().errorStream()
    << "Failed to connect to the server after " << max_retries << " attempts.";
  close(server_fd);
  throw std::runtime_error("Failed to connect to the server");
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
  } catch (...) {
    close(server_fd);
    throw;
  }
}

/**
 * @brief 向server发送序列化后的请求
 *
 * @param req 请求对象
 *
 */
void Socket::send(const ReqPtr& req) {
  static Json::StreamWriterBuilder writer;

  std::string data = Json::writeString(writer, req->toJson());
  uint32_t dataSize = data.size();
  data =
    std::string(reinterpret_cast<char*>(&dataSize), sizeof(dataSize)) + data;

  ssize_t bytesSent = ::send(server_fd, data.c_str(), data.size(), 0);

  if (bytesSent == -1) {
    if (errno == EPIPE) {  // 检测到 "Broken pipe" 错误
      log4cpp::Category::getRoot().errorStream()
        << "Failed to send request: Broken pipe. Attempting to reconnect...";

      close(server_fd);
      try {
        reconnect();  // 尝试重新连接
        bytesSent =
          ::send(server_fd, data.c_str(), data.size(), 0);  // 重新发送请求
        if (bytesSent == -1) {
          throw std::runtime_error("Failed to send request after reconnecting");
        }
      } catch (const std::exception& e) {
        handleNotify("Failed to reconnect: " + std::string(e.what()));
        throw std::runtime_error("Reconnection failed");
      }
    } else {
      handleNotify("Failed to send request: " + std::string(strerror(errno)));
      throw std::runtime_error("Failed to send request");
    }
  }
}

/**
 * @brief 从server接收并解析response
 *
 * @return ResPtr 解析出的response
 *
 */
ResPtr Socket::receive() {
  read_buffer.resize(mp::MAX_MESSAGE_SIZE);  // 预留空间

  ssize_t bytesRead = read(server_fd, read_buffer.data(), mp::MAX_MESSAGE_SIZE);

  if (bytesRead > 0) {
    read_buffer.resize(bytesRead);  // 调整缓冲区大小以适应实际读取的数据
  }

  // 连接是否关闭
  if (bytesRead == 0) {
    // 断开连接
    close(server_fd);

    throw std::runtime_error(
      "Server " + std::to_string(server_fd) + " disconnect"
    );
  }

  // socket是否产生了错误
  if (bytesRead < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      close(server_fd);

      // 没有更多数据可读
      throw std::runtime_error(
        "Server " + std::to_string(server_fd) + "is broken, disconnect now"
      );
    }
    // todo

    close(server_fd);
    // 未知错误
    throw std::runtime_error(
      "Server " + std::to_string(server_fd) +
      " is broken for unknown reason, disconnect now"
    );
  }
  // read_buffer仍然有内容
  for (const uint8_t byte : read_buffer) {
    switch (state) {
      case ReceiveStatus::READ_DATA_SIZE:
        readDataSize(byte);
        break;
      case ReceiveStatus::READ_DATA:
        if (readData(byte)) {
          handleRemoteResponse(parseJsonFromBuffer());
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

    return jsonData;
  }
  write_buffer.clear();
  throw std::runtime_error("Illegal json format");
}

}  // namespace zipfiles::client