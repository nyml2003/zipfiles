#include "server/acceptor.h"
#include <future>
#include <iostream>
#include <sys/epoll.h>
#include "server/handler.h"
#include "server/socket/socket.h"

namespace zipfiles::server {

/**
 * @brief 当前正在运行的handler数
 * @details 因为只有一个acceptor，所以不需要做并发处理
 *
 */
int threadCount = 0;

void doAccept() {
  int serverFd = ServerSocket::getServerFd();

  fd_set readfds;
  std::vector<std::future<void>> futures;  // 用于存储 future 对象

  // 初始化fd
  FD_ZERO(&readfds);
  FD_SET(serverFd, &readfds);

  // 初始化epoll实例
  int epollFd = epoll_create1(0);
  if (epollFd == -1) {
    std::cerr << "Failed to create epoll file descriptor" << std::endl;
  }

  // 初始化epoll事件
  struct epoll_event event {};
  event.events = EPOLLIN;  // 监听可读事件
  event.data.fd = serverFd;
  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event) == -1) {
    std::cerr << "Failed to add file descriptor to epoll" << std::endl;
    close(epollFd);
  }

  while (true) {
    std::array<struct epoll_event, 8> events{};
    int numEvents = epoll_wait(epollFd, events.data(), events.size(), -1);
    if (numEvents == -1) {
      std::cerr << "epoll_wait error" << std::endl;
      break;
    }

    // ! deprecated
    // int activity = select(serverFd + 1, &readfds, nullptr, nullptr, nullptr);
    // if (activity < 0) {
    //   std::cerr << "Select error" << std::endl;
    //   break;
    // }

    for (int i = 0; i < numEvents; ++i) {
      if (events[i].data.fd == serverFd && threadCount < MAX_THREADS) {
        ServerSocket::acceptConnection();

        futures.emplace_back(std::async(std::launch::async, doHandle));

        threadCount++;
      }
    }
  }
}

}  // namespace zipfiles::server
