#include <arpa/inet.h>
#include <sys/epoll.h>
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/mp.h"
#include "server/socket/socket.h"

/**
 * @brief 当前正在运行的连接数
 * @details 因为只有一个acceptor，所以不需要做并发处理
 *
 */
int connectionCount = 0;

namespace zipfiles::server {
Socket::Socket()
  : server_fd(socket(AF_INET, SOCK_STREAM, 0)), address{}, connectionCount(0) {
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

  // 限制最大监听数量
  if (listen(server_fd, MAX_CONNECTIONS) < 0) {
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

void Socket::acceptConnection(int epoll_fd) {
  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(
    getInstance().server_fd, reinterpret_cast<struct sockaddr*>(&client_addr),
    &client_len
  );

  // 创建client_fd失败
  if (client_fd < 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to accept connection.";
    perror("accept");
    exit(EXIT_FAILURE);
  } else {
    std::string ip(inet_ntoa(client_addr.sin_addr));
    std::string port(std::to_string(ntohs(client_addr.sin_port)));

    log4cpp::Category::getRoot().infoStream()
      << "Accept connection with ip: " << ip << " port: " << port
      << " client_fd: " << client_fd;
  }

  // 将client_fd加入epoll
  struct epoll_event event {};
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = client_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to add client_fd " << client_fd << " into epoll, which fd is "
      << epoll_fd;
    perror("epoll_ctl: client_fd");
    close(client_fd);
  } else {
    log4cpp::Category::getRoot().infoStream()
      << "Add client_fd " << client_fd << " to epoll_fd " << epoll_fd;

    getInstance().connectionCount++;
  }
}

ReqPtr Socket::receive(int client_fd) {
  std::array<char, mp::MAX_MESSAGE_SIZE> buffer = {0};

  ssize_t valread = read(client_fd, buffer.data(), mp::MAX_MESSAGE_SIZE);

  if (valread == 0) {
    close(client_fd);
    getInstance().connectionCount--;
    log4cpp::Category::getRoot().infoStream()
      << "Client disconnect, which has client_fd " << client_fd;
    log4cpp::Category::getRoot().infoStream()
      << "Connection count: " << Socket::getConnectionCount();
    throw std::runtime_error("Client disconnected");
  }

  if (valread < 0) {
    perror("Failed to receive request");
    close(client_fd);
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

void Socket::send(int client_fd, const ResPtr& res) {
  static Json::StreamWriterBuilder writer;

  std::string data = Json::writeString(writer, res->toJson());
  ::send(client_fd, data.c_str(), data.size(), 0);
}

int Socket::getConnectionCount() {
  return getInstance().connectionCount;
}
}  // namespace zipfiles::server
