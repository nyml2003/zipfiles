#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include "server/acceptor.h"
int main() {
  // zipfiles::server::daemonize();  // 初始化守护进程
  try {
    // 假设配置文件名为 "log4cpp.properties"
    std::string configFilePath = "/app/config/server.log.properties";
    log4cpp::PropertyConfigurator::configure(configFilePath);
    log4cpp::Category::getRoot().infoStream() << "Server started";
  } catch (log4cpp::ConfigureFailure& f) {
    std::cerr << "Configure Problem: " << f.what() << std::endl;
  }
  zipfiles::server::doAccept();

  return 0;
}