#include "server/socket/socket.h"

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>
#include <string>

#include "log4cpp/Category.hh"

namespace zipfiles::server {
/**
 * @brief server socket初始化
 *
 */
Socket::Socket() : server_fd(socket(AF_INET, SOCK_STREAM, 0)), address{} {
  if (server_fd == -1) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to initialize server socket";

    exit(EXIT_FAILURE);
  }

  // 若程序退出则立刻释放socket
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // 非阻塞
  int flags = fcntl(server_fd, F_GETFL, 0);
  fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);  // NOLINT

  // 初始化实例
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  addrlen = sizeof(address);

  // 绑定socket
  if (bind(
        server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)
      ) < 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to bind server socket";
    close(server_fd);

    exit(EXIT_FAILURE);
  }

  // 限制最大连接数量
  if (listen(server_fd, MAX_CONNECTIONS) < 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to listen server socket";
    close(server_fd);

    exit(EXIT_FAILURE);
  }
}

Socket::~Socket() {
  close(server_fd);
}

/**
 * @brief 获取当前的server fd
 *
 * @return int server fd
 *
 */
int Socket::getServerFd() {
  return getInstance().server_fd;
}

/**
 * @brief 接受server fd的连接请求，把连接交给epoll实例管理
 *
 * @param epoll_fd epoll实例
 *
 */
void Socket::acceptConnection(int epoll_fd) {
  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(
    getInstance().server_fd, reinterpret_cast<struct sockaddr*>(&client_addr),
    &client_len
  );

  // 创建client_fd失败
  if (client_fd < 0) {
    throw std::runtime_error(
      "Failed to accept connection" +
      std::string(inet_ntoa(client_addr.sin_addr))
    );
  }

  std::string ip(inet_ntoa(client_addr.sin_addr));
  std::string port(std::to_string(ntohs(client_addr.sin_port)));

  log4cpp::Category::getRoot().infoStream()
    << "Accept connection with ip: " << ip << " port: " << port
    << " client_fd: " << client_fd;

  // 设置非阻塞
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);  // NOLINT

  // 将client_fd加入epoll
  struct epoll_event event {};
  event.events = EPOLLIN;
  event.data.fd = client_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
    close(client_fd);
    throw std::runtime_error(
      "Failed to add client_fd " + std::to_string(client_fd) +
      " into epoll, its epoll_fd is " + std::to_string(epoll_fd)
    );
  }

  log4cpp::Category::getRoot().infoStream()
    << "Add client_fd " << client_fd << " to epoll_fd " << epoll_fd;
}

/**
 * @brief 读取socket buffer的数据，最大大小为MAX_MESSAGE_SIZE
 *
 * @param client_fd client
 *
 * @param read_buffer 将数据读取到read_buffer
 *
 */
void Socket::receive(int client_fd, std::vector<uint8_t>& read_buffer) {
  read_buffer.resize(MAX_MESSAGE_SIZE);  // 预留空间

  ssize_t bytesRead = read(client_fd, read_buffer.data(), MAX_MESSAGE_SIZE);

  if (bytesRead > 0) {
    read_buffer.resize(bytesRead);  // 调整缓冲区大小以适应实际读取的数据
  }

  // 连接是否关闭
  if (bytesRead == 0) {
    // 断开连接
    close(client_fd);

    throw std::runtime_error(
      "Client " + std::to_string(client_fd) + " disconnect"
    );
  }

  // socket是否产生了错误
  if (bytesRead < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      close(client_fd);

      // 没有更多数据可读
      throw std::runtime_error(
        "Client " + std::to_string(client_fd) + "is broken, disconnect now"
      );
    }

    close(client_fd);
    // 未知错误
    throw std::runtime_error(
      "Client " + std::to_string(client_fd) +
      " is broken for unknown reason, disconnect now"
    );
  }
}

/**
 * @brief 向指定的client发送response
 *
 * @param client_fd client
 *
 * @param res reponse
 *
 */
void Socket::send(int client_fd, const Res& res) {
  static Json::StreamWriterBuilder writer;

  std::string data = Json::writeString(writer, res.toJson());
  log4cpp::Category::getRoot().infoStream()
    << "Send response: " << data << " to client_fd: " << client_fd;
  uint32_t dataSize = data.size();
  data =
    std::string(reinterpret_cast<char*>(&dataSize), sizeof(dataSize)) + data;

  ::send(client_fd, data.c_str(), data.size(), 0);
}

void Socket::send(int client_fd, const Notification& notification) {
  static Json::StreamWriterBuilder writer;

  std::string data = Json::writeString(writer, notification.toJson());
  log4cpp::Category::getRoot().infoStream()
    << "Send notification: " << data << " to client_fd: " << client_fd;
  uint32_t dataSize = data.size();
  data =
    std::string(reinterpret_cast<char*>(&dataSize), sizeof(dataSize)) + data;

  ::send(client_fd, data.c_str(), data.size(), 0);
}

void sendError(int client_fd, const std::string& message) {
  auto notification = Notification(message, zipfiles::Code::ERROR);
  Socket::send(client_fd, notification);
}

}  // namespace zipfiles::server
