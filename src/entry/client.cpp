
#include <fstream>
#include "client/launcher.h"
using zipfiles::client::Launcher;
int main(int argc, char* argv[]) {
  // Debug
  // 因为会导致Debug冲突，因此暂时先手动处理
  std::ofstream ofs;
  ofs.open("/app/bin/client.log", std::ofstream::out | std::ofstream::trunc);
  ofs.close();

  Launcher::getInstance().startLogger();
  Launcher::getInstance().startReciever();
  Launcher::getInstance().startGTK(argc, argv);

  return 0;
}