#include <sys/epoll.h>
#include <unistd.h>
#include <future>
#include <vector>
#include "log4cpp/Category.hh"
#include "server/acceptor.h"
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
  int serverFd = Socket::getServerFd();

  fd_set readfds;
  std::vector<std::future<void>> futures;  // 用于存储 future 对象

  // 初始化fd
  FD_ZERO(&readfds);
  FD_SET(serverFd, &readfds);

  // 初始化epoll实例
  int epollFd = epoll_create1(0);
  if (epollFd == -1) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to create epoll file descriptor";
    return;
  }

  // 初始化epoll事件
  struct epoll_event event {};
  event.events = EPOLLIN;  // 监听可读事件
  event.data.fd = serverFd;
  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event) == -1) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to add file descriptor to epoll";
    close(epollFd);
  }

  while (true) {
    // 设置epoll_wait
    std::array<struct epoll_event, 8> events{};
    int numEvents = epoll_wait(epollFd, events.data(), events.size(), -1);
    if (numEvents == -1) {
      log4cpp::Category::getRoot().errorStream() << "Failed to wait for events";
      break;
    }

    log4cpp::Category::getRoot().infoStream()
      << "Number of events: " << numEvents;  // 增加：输出事件数量

    // 检查是否是server Fd
    for (int i = 0; i < numEvents; ++i) {
      if (events[i].data.fd == serverFd && threadCount < MAX_THREADS) {
        Socket::acceptConnection();

        futures.emplace_back(std::async(std::launch::async, doHandle));

        // 增加当前正在运行的线程数
        threadCount++;
        log4cpp::Category::getRoot().infoStream()
          << "Thread count: " << threadCount;
      } else {
        // todo: thread数量过多的处理
        log4cpp::Category::getRoot().warnStream()
          << "Max threads reached, cannot accept new connection";  // 增加：输出线程数达到上限的警告
      }
    }

    // todo: future回调
  }
}

}  // namespace zipfiles::server
