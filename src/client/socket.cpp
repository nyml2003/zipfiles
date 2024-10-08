#include "client/socket.h"
#include <arpa/inet.h>
#include <thread>
#include "client/view.h"
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/mp.h"

namespace zipfiles::client {

void Socket::initializeSocket() {
  log4cpp::Category::getRoot().infoStream() << "Initializing socket...";
  sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
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
    close(sock);
    throw std::runtime_error("Invalid address/ Address not supported");
  }
}

Socket::Socket() : sock(-1), serv_addr{} {
  try {
    initializeSocket();
    connectWithRetries();
  } catch (const std::exception& e) {
    close(sock);
    throw e;
  }
}

void Socket::connectWithRetries() {
  const int max_retries = 5;
  int retries = 0;
  while (retries < max_retries) {
    if (connect(
          sock, reinterpret_cast<struct sockaddr*>(&serv_addr),
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
  close(sock);
  throw std::runtime_error("Failed to connect to the server");
}

Socket::~Socket() {
  close(sock);
}

void Socket::reconnect() {
  close(sock);
  try {
    initializeSocket();
    connectWithRetries();
  } catch (...) {
    close(sock);
    throw;
  }
}

void Socket::send(const ReqPtr& req) {
  static Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, req->toJson());
  ssize_t bytesSent = ::send(sock, data.c_str(), data.size(), 0);

  if (bytesSent == -1) {
    if (errno == EPIPE) {  // 检测到 "Broken pipe" 错误
      log4cpp::Category::getRoot().errorStream()
        << "Failed to send request: Broken pipe. Attempting to reconnect...";
      close(sock);
      try {
        reconnect();  // 尝试重新连接
        bytesSent = ::send(sock, data.c_str(), data.size(), 0);  // 重新发送请求
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

ResPtr Socket::receive() const {
  std::array<char, mp::MAX_MESSAGE_SIZE> buffer = {0};
  ssize_t valread = read(sock, buffer.data(), mp::MAX_MESSAGE_SIZE);

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

}  // namespace zipfiles::client