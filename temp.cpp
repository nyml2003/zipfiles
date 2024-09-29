#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define MAX_EVENTS 10
#define PORT 8080

// 设置文件描述符为非阻塞模式
void setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 处理新连接
void handleNewConnection(int epoll_fd, int listen_fd) {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(
    listen_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len
  );
  if (client_fd == -1) {
    perror("accept");
    return;
  }

  setNonBlocking(client_fd);

  struct epoll_event event;
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = client_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
    perror("epoll_ctl: client_fd");
    close(client_fd);
    return;
  }

  std::cout << "Accepted connection from " << inet_ntoa(client_addr.sin_addr)
            << std::endl;
}

// 处理客户端数据
void handleClientData(int client_fd) {
  char buffer[1024];
  ssize_t count = read(client_fd, buffer, sizeof(buffer));
  if (count == -1) {
    if (errno != EAGAIN) {
      perror("read");
      close(client_fd);
    }
  } else if (count == 0) {
    close(client_fd);
  } else {
    std::cout << "Received: " << std::string(buffer, count) << std::endl;
    // Echo the data back to the client
    write(client_fd, buffer, count);
  }
}

int main() {
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd == -1) {
    perror("socket");
    return 1;
  }

  setNonBlocking(listen_fd);

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(listen_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
    perror("bind");
    close(listen_fd);
    return 1;
  }

  if (listen(listen_fd, SOMAXCONN) == -1) {
    perror("listen");
    close(listen_fd);
    return 1;
  }

  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1");
    close(listen_fd);
    return 1;
  }

  struct epoll_event event;
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = listen_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
    perror("epoll_ctl: listen_fd");
    close(listen_fd);
    close(epoll_fd);
    return 1;
  }

  struct epoll_event events[MAX_EVENTS];

  while (true) {
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; ++i) {
      if (events[i].data.fd == listen_fd) {
        handleNewConnection(epoll_fd, listen_fd);
      } else {
        handleClientData(events[i].data.fd);
      }
    }
  }

  close(listen_fd);
  close(epoll_fd);
  return 0;
}
