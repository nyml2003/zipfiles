#include <unistd.h>
#include <csignal>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include "server/acceptor.h"

namespace zipfiles::server {

/**
 * @brief 守护进程初始化函数
 * ? 待使用
 */
void daemonize() {
  pid_t pid = fork();
  if (pid < 0) {
    log4cpp::Category::getRoot().errorStream() << "Failed to fork process";
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);  // 父进程退出
  }

  // 创建新的会话
  if (setsid() < 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to create new session";
    exit(EXIT_FAILURE);
  }

  // 捕捉和处理信号
  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to set signal handler for SIGCHLD";
    exit(EXIT_FAILURE);
  }
  if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to set signal handler for SIGTSTP";
    exit(EXIT_FAILURE);
  }

  // 更改工作目录
  if (chdir("/") < 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to change working directory";
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
  try {
    // 假设配置文件名为 "log4cpp.properties"
    std::string configFilePath = "/app/bin/server/server.log.properties";
    log4cpp::PropertyConfigurator::configure(configFilePath);
    log4cpp::Category::getRoot().infoStream() << "Server started";
  } catch (log4cpp::ConfigureFailure& f) {
    std::cerr << "Configure Problem: " << f.what() << std::endl;
  }
  zipfiles::server::doAccept();

  return 0;
}