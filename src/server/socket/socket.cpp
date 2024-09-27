#include "server/socket/socket.h"
#include "mp/mp.h"
#include "mp/Request.h"
#include "mp/Response.h"
namespace zipfiles::server {
Socket::Socket()
  : server_fd(socket(AF_INET, SOCK_STREAM, 0)), client_fd(0), address{} {
  if (server_fd == -1) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
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

Socket::~Socket() {
  close(server_fd);
}

int Socket::getServerFd() {
  return getInstance().server_fd;
}

void Socket::acceptConnection() {
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

ReqPtr Socket::receive() {
  std::array<char, mp::MAX_MESSAGE_SIZE> buffer = {0};
  ssize_t valread =
    read(getInstance().client_fd, buffer.data(), mp::MAX_MESSAGE_SIZE);
  if (valread == 0) {
    close(getInstance().client_fd);
    throw std::runtime_error("Client disconnected");
  }
  if (valread < 0) {
    perror("Failed to receive request");
    close(getInstance().client_fd);
  }
  static Json::CharReaderBuilder reader;
  Json::Value jsonData;
  std::string errs;
  std::istringstream stream(buffer.data());
  if (Json::parseFromStream(reader, stream, &jsonData, &errs)) {
    return Req::fromJson(jsonData);
  }
  throw std::runtime_error("Failed to parse request");
}

void Socket::send(const ResPtr& res) {
  static Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, res->toJson());
  ::send(getInstance().client_fd, data.c_str(), data.size(), 0);
}
}  // namespace zipfiles::server