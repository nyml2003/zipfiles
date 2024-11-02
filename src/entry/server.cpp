#include <fstream>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include "server/acceptor.h"
#include "server/configure/configure.h"
#include "server/tools/committable.h"

int main() {
  // 启动logger
  try {
    // Debug
    // 因为会导致Debug冲突，因此暂时先手动处理
    std::ofstream ofs;
    ofs.open("/app/bin/server.log", std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    // 假设配置文件名为 "log4cpp.properties"
    std::string configFilePath = "/app/config/server.log.properties";
    log4cpp::PropertyConfigurator::configure(configFilePath);
    log4cpp::Category::getRoot().infoStream() << "Server started";
  } catch (log4cpp::ConfigureFailure& f) {
    std::cerr << "Configure Problem: " << f.what() << std::endl;
  }

  // 启动acceptor
  zipfiles::server::CommitTable::readCommitTable(
    zipfiles::server::COMMIT_TABLE_PATH
  );
  zipfiles::server::doAccept();

  return 0;
}