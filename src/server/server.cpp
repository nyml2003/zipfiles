#include <iostream>
#include <unistd.h>
#include <csignal>

#include "server/acceptor.h"
#include "server/handler.h"

namespace zipfiles::server {

/**
 * @brief 守护进程初始化函数
 * ? 待使用
 */
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

}  // namespace zipfiles::server

int main() {
  // zipfiles::server::daemonize();  // 初始化守护进程
  zipfiles::server::doAccept();
  
  return 0;
}