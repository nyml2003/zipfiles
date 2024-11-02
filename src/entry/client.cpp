#include <fstream>
#include <thread>
#include "client/launcher.h"
#include "client/socket.h"

int main(int argc, char* argv[]) {
  // Debug
  // 因为会导致Debug冲突，因此暂时先手动处理
  std::ofstream ofs;
  ofs.open("/app/bin/client.log", std::ofstream::out | std::ofstream::trunc);
  ofs.close();

  zipfiles::client::Launcher::startLogger();
  std::thread socketReceiver([]() {
    while (true) {
      zipfiles::client::Socket::getInstance().receive();
    }
  });
  zipfiles::client::Launcher::run(argc, argv);
  return 0;
}