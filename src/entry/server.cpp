#include <fstream>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include "server/configure/configure.h"
#include "server/socket/acceptor.h"
#include "server/tools/committable.h"

int main() {
  // 启动logger
  try {
    // 假设配置文件名为 "log4cpp.properties"
    std::string configFilePath =
      "/usr/local/zipfiles/.zip/server.log.properties";
    log4cpp::PropertyConfigurator::configure(configFilePath);
    log4cpp::Category::getRoot().infoStream() << "Server started";
  } catch (log4cpp::ConfigureFailure& f) {
    std::cerr << "Configure Problem: " << f.what() << std::endl;
  }

  // 初始化commit_table
  zipfiles::server::CommitTable::readCommitTable(
    zipfiles::server::COMMIT_TABLE_PATH
  );

  // 启动acceptor
  zipfiles::server::doAccept();

  return 0;
}