#include "server/acceptor.h"
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>
#include <exception>
#include "log4cpp/Category.hh"
#include "server/selector.h"
#include "server/socket/socket.h"

namespace zipfiles::server {

/**
 * @brief 启动acceptor
 *
 */
void doAccept() {
  // 初始化server_fd
  int server_fd = Socket::getServerFd();
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(server_fd, &readfds);

  // 初始化epoll实例
  int epollFd = epoll_create1(0);
  if (epollFd == -1) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to create epoll file descriptor";
    return;
  }

  // 初始化epoll事件
  struct epoll_event event {};
  event.events = EPOLLIN | EPOLLET;  // 监听可读事件
  event.data.fd = server_fd;
  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to add file descriptor to epoll";
    close(epollFd);
  }

  // 事件数组
  std::array<struct epoll_event, MAX_EPOLL_EVENTS> events{};

  // selector实例
  Selector selector;

  while (true) {
    // 调用epoll_wait
    int numEvents = epoll_wait(epollFd, events.data(), events.size(), -1);

    // epoll错误
    if (numEvents == -1) {
      log4cpp::Category::getRoot().errorStream() << "Failed to wait for events";
      exit(EXIT_FAILURE);
    }

    // 检查是否是server_fd
    for (int i = 0; i < numEvents; ++i) {
      if (events[i].data.fd == server_fd) {
        log4cpp::Category::getRoot().infoStream()
          << "Now processing event from " << events[i].data.fd
          << "(server_fd) as accept event";

        int count = selector.getConnectionCount();

        if (count > MAX_CONNECTIONS) {
          log4cpp::Category::getRoot().warnStream()
            << "Max connections reached, cannot accept new connection";  // 增加：输出连接数达到上限的警告

          continue;
        }

        try {
          Socket::acceptConnection(epollFd);
        } catch (std::exception& e) {
          log4cpp::Category::getRoot().errorStream()
            << "Failed to accept connection, because " << e.what();

          continue;
        }

        selector.addConnectionCount();

        // 当前正在运行的连接数
        log4cpp::Category::getRoot().infoStream()
          << "Connection count: " << count + 1;
      } else {
        // 让selector分发任务
        try {
          int client_fd = events[i].data.fd;
          selector.doSelect(client_fd);
        } catch (std::exception& e) {
          log4cpp::Category::getRoot().errorStream()
            << "Failed to dispatch task, because " << e.what();
        }
      }
    }
  }
}

}  // namespace zipfiles::server
