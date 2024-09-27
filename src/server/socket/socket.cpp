#include "server/socket/socket.h"
#include <iostream>
#include "mp/mp.h"
namespace zipfiles::server {
ServerSocket::ServerSocket()
  : server_fd(socket(AF_INET, SOCK_STREAM, 0)), client_fd(0), address{} {
  if (server_fd == -1) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // 若程序退出则立刻释放socket
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // 初始化实例
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(mp::PORT);
  addrlen = sizeof(address);

  if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
}

ServerSocket::~ServerSocket() {
  close(server_fd);
}

int ServerSocket::getServerFd() {
  return getInstance().server_fd;
}

void ServerSocket::acceptConnection() {
  getInstance().client_fd = accept(
    getInstance().server_fd,
    reinterpret_cast<struct sockaddr*>(&getInstance().address),
    reinterpret_cast<socklen_t*>(&getInstance().addrlen)
  );
  if (getInstance().client_fd < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
}

bool ServerSocket::receive(const mp::RequestPtr& req) {
  std::array<char, mp::MAX_MESSAGE_SIZE> buffer = {0};
  ssize_t valread =
    read(getInstance().client_fd, buffer.data(), mp::MAX_MESSAGE_SIZE);
  if (valread == 0) {
    std::cout << "Client disconnected." << std::endl;
    close(getInstance().client_fd);
    return false;
  }
  if (valread < 0) {
    perror("Failed to receive request");
    return false;
  }
  std::cout << "Received data: " << buffer.data() << std::endl;
  static Json::CharReaderBuilder reader;
  Json::Value jsonData;
  std::string errs;
  std::istringstream stream(buffer.data());
  if (Json::parseFromStream(reader, stream, &jsonData, &errs)) {
    req->fromJson(jsonData);
    return true;
  }
  return false;
}

bool ServerSocket::send(const mp::ResponsePtr& res) {
  static Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, res->toJson());
  std::cout << "Sending response: " << data << std::endl;
  ::send(getInstance().client_fd, data.c_str(), data.size(), 0);
  return true;
}
}  // namespace zipfiles::server