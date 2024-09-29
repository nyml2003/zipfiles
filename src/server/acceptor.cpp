#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <future>
#include <vector>
#include "log4cpp/Category.hh"
#include "server/acceptor.h"
#include "server/handler.h"
#include "server/socket/socket.h"

namespace zipfiles::server {

void doAccept() {
  fd_set readfds;
  std::vector<std::future<void>> futures;  // 用于存储 future 对象

  // 初始化server_fd
  int server_fd = Socket::getServerFd();

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

  std::array<struct epoll_event, MAX_EPOLL_EVENTS> events{};

  while (true) {
    // 调用epoll_wait
    log4cpp::Category::getRoot().infoStream() << "Waiting for events...";
    int numEvents = epoll_wait(epollFd, events.data(), events.size(), -1);

    if (numEvents == -1) {
      log4cpp::Category::getRoot().errorStream() << "Failed to wait for events";
      break;
    }

    log4cpp::Category::getRoot().infoStream()
      << "Number of events: " << numEvents;  // 增加：输出事件数量

    // 检查是否是server Fd
    for (int i = 0; i < numEvents; ++i) {
      if (events[i].data.fd == server_fd) {
        log4cpp::Category::getRoot().infoStream()
          << "Now processing event from " << events[i].data.fd
          << "(server_fd) as accept event.";

        if (Socket::getConnectionCount() > MAX_CONNECTIONS) {
          // todo: connection数量过多的处理
          // ? 不处理，前端直接不展示数据了
          log4cpp::Category::getRoot().warnStream()
            << "Max threads reached, cannot accept new connection";  // 增加：输出连接数达到上限的警告

          continue;
        }

        Socket::acceptConnection(epollFd);

        // 当前正在运行的连接数
        log4cpp::Category::getRoot().infoStream()
          << "Connection count: " << Socket::getConnectionCount();
      } else {
        // 不是server_fd，那么就是client_fd
        log4cpp::Category::getRoot().infoStream()
          << "Now processing event from " << events[i].data.fd
          << " as handle event.";
        doHandle(events[i].data.fd);
      }
    }

    // todo: future回调
    for (auto& future : futures) {
      future.get();
      log4cpp::Category::getRoot().infoStream() << "Getting future result";
    }
  }
}

}  // namespace zipfiles::server
