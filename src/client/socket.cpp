#include "client/socket.h"
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include "mp/mp.h"
#include "client/view.h"
#include "mp/Response.h"
#include "mp/Request.h"

namespace zipfiles::client::socket {
Socket::Socket() : sock(::socket(AF_INET, SOCK_STREAM, 0)), serv_addr{} {
  if (sock < 0) {
    perror("Socket creation error");
    throw std::runtime_error("Socket creation error");
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(mp::PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    close(sock);
    throw std::runtime_error("Invalid address/ Address not supported");
  }

  const int max_retries = 5;
  int retries = 0;
  while (retries < max_retries) {
    if (connect(sock, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
      perror("Connection Failed");
      retries++;
      std::cerr << "Retrying connection (" << retries << "/" << max_retries
                << ")..." << std::endl;
      // 已经重试了retries次，仍然失败
      if (retries == 1) {
        client::view::handleFatal(
          std::runtime_error("Failed to connect to the server")
        );
      } else {
        client::view::handleFatal(std::runtime_error(
          "Failed to connect to the server after " + std::to_string(retries) +
          " attempts."
        ));
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待1秒后重试
    } else {
      break;
    }
  }

  if (retries == max_retries) {
    std::cerr << "Failed to connect to the server after " << max_retries
              << " attempts." << std::endl;
    close(sock);
    throw std::runtime_error("Failed to connect to the server");
  }
}

Socket::~Socket() {
  close(sock);
}

void Socket::reconnect() {
  // 关闭旧的 socket
  close(getInstance().sock);

  // 创建新的 socket 并重新连接
  getInstance().sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (getInstance().sock < 0) {
    throw std::runtime_error("Failed to create socket");
  }

  int retries = 0;
  const int max_retries = 5;
  while (retries < max_retries) {
    if (connect(getInstance().sock, reinterpret_cast<struct sockaddr*>(&(getInstance().serv_addr)), sizeof(getInstance().serv_addr)) == 0) {
      return;
    }
    retries++;
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待一段时间后重试
  }

  throw std::runtime_error("Failed to reconnect to the server");
}

void Socket::send(const ReqPtr& req) {
  static Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, req->toJson());
  ssize_t bytesSent = ::send(getInstance().sock, data.c_str(), data.size(), 0);

  if (bytesSent == -1) {
    if (errno == EPIPE) {  // 检测到 "Broken pipe" 错误
      std::cerr
        << "Failed to send request: Broken pipe. Attempting to reconnect..."
        << std::endl;
      close(getInstance().sock);
      try {
        reconnect();  // 尝试重新连接
        bytesSent = ::send(
          getInstance().sock, data.c_str(), data.size(), 0
        );  // 重新发送请求
        if (bytesSent == -1) {
          std::cerr << "Failed to send request after reconnecting."
                    << std::endl;
          throw std::runtime_error("Failed to send request after reconnecting");
        }
      } catch (const std::exception& e) {
        std::cerr << "Reconnection failed: " << e.what() << std::endl;
        throw std::runtime_error("Reconnection failed");
      }
    } else {
      std::cerr << "Failed to send request: " << strerror(errno) << std::endl;
      throw std::runtime_error("Failed to send request");
    }
  }
}

ResPtr Socket::receive() {
  std::array<char, mp::MAX_MESSAGE_SIZE> buffer = {0};
  ssize_t valread =
    read(getInstance().sock, buffer.data(), mp::MAX_MESSAGE_SIZE);

  if (valread > 0) {
    static Json::CharReaderBuilder reader;
    Json::Value jsonData;
    std::string errs;
    std::istringstream stream(buffer.data());
    if (Json::parseFromStream(reader, stream, &jsonData, &errs)) {
      return Res::fromJson(jsonData);
    }
  }
  throw std::runtime_error("Failed to receive response");
}
}  // namespace zipfiles::client::socket