#include <iostream>
#include <vector>
#include <future>
#include <sys/select.h>
#include <unistd.h>
#include <csignal>
#include "server/api/api.h"
#include "mp/mp.h"

namespace zipfiles::server {

// 守护进程初始化
void daemonize() {
  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Fork failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);  // 父进程退出
  }

  // 创建新的会话
  if (setsid() < 0) {
    std::cerr << "Setsid failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 捕捉和处理信号
  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    std::cerr << "Failed to set signal handler for SIGCHLD" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
    std::cerr << "Failed to set signal handler for SIGTSTP" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 更改工作目录
  if (chdir("/") < 0) {
    std::cerr << "Chdir failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 关闭文件描述符
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

void handleClient() {
  mp::RequestPtr request = std::make_shared<mp::Request>();

  try {
    while (mp::ServerSocket::receive(request)) {

      if (request->is(mp::ApiType::GET_FILE_LIST)) {
        mp::ResponsePtr response = std::make_shared<mp::Response>();

        mp::GetFileListRequestPtr getFileListRequest =
          std::make_shared<mp::GetFileListRequest>();

        getFileListRequest->fromJson(request->getPayload());

        mp::GetFileListResponsePtr getFileListResponse =
          api::getFileList(getFileListRequest);

        response->setStatus(mp::StatusCode::OK);

        response->setPayload(getFileListResponse);

        if (!mp::ServerSocket::send(response)) {
          std::cerr << "Failed to send response." << std::endl;
        }
      }

      if (request->is(mp::ApiType::GET_FILE_DETAIL)) {

        mp::ResponsePtr response = std::make_shared<mp::Response>();

        mp::GetFileDetailRequestPtr getFileDetailRequest =
          std::make_shared<mp::GetFileDetailRequest>();

        getFileDetailRequest->fromJson(request->getPayload());

        mp::GetFileDetailResponsePtr getFileDetailResponse =
          api::getFileDetail(getFileDetailRequest);
        
        response->setStatus(mp::StatusCode::OK);

        response->setPayload(getFileDetailResponse);

        if (!mp::ServerSocket::send(response)) {
          std::cerr << "Failed to send response." << std::endl;
        }
      }
      
    }
  } catch (const std::exception& e) {

    std::cerr << e.what() << std::endl;

  }
}

void run() {
  int serverFd = mp::ServerSocket::getServerFd();

  fd_set readfds;
  std::vector<std::future<void>> futures;  // 用于存储 future 对象

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(serverFd, &readfds);

    int activity = select(serverFd + 1, &readfds, nullptr, nullptr, nullptr);
    if (activity < 0) {
      std::cerr << "Select error" << std::endl;
      break;
    }

    if (FD_ISSET(serverFd, &readfds)) {
      mp::ServerSocket::acceptConnection();

      futures.emplace_back(std::async(std::launch::async, handleClient));

      std::cout << "Waiting for new client connection..." << std::endl;
      
    }
  }
}

}  // namespace zipfiles::server

int main() {
  // zipfiles::server::daemonize();  // 初始化守护进程
  zipfiles::server::run();
  return 0;
}