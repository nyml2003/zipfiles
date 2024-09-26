#include "client/socket.h"
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include "mp/mp.h"
#include "client/view.h"

namespace zipfiles::client::socket {
ClientSocket::ClientSocket()
  : sock(::socket(AF_INET, SOCK_STREAM, 0)), serv_addr{} {
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

ClientSocket::~ClientSocket() {
  close(sock);
}

bool ClientSocket::send(const mp::RequestPtr& req) {
  static Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, req->toJson());
  std::cout << "Sending request: " << data << std::endl;
  ssize_t bytesSent = ::send(getInstance().sock, data.c_str(), data.size(), 0);
  if (bytesSent == -1) {
    std::cerr << "Failed to send request: " << strerror(errno) << std::endl;
    if (errno == ECONNREFUSED) {
      std::cerr << "Connection refused. Please check if the server is running "
                   "and reachable."
                << std::endl;
    }
    return false;
  }
  return true;
}

bool ClientSocket::receive(const mp::ResponsePtr& res) {
  std::array<char, mp::MAX_MESSAGE_SIZE> buffer = {0};
  ssize_t valread =
    read(getInstance().sock, buffer.data(), mp::MAX_MESSAGE_SIZE);

  std::cout << "Received data: " << buffer.data() << std::endl;
  if (valread > 0) {
    static Json::CharReaderBuilder reader;
    Json::Value jsonData;
    std::string errs;
    std::istringstream stream(buffer.data());
    if (Json::parseFromStream(reader, stream, &jsonData, &errs)) {
      res->fromJson(jsonData);
      return true;
    }
  }
  return false;
}
}  // namespace zipfiles::client::socket