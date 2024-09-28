#include "client/launcher.h"

int main(int argc, char* argv[]) {
  zipfiles::client::launcher::Launcher::startLogger();
  zipfiles::client::launcher::Launcher::run(argc, argv);
  return 0;
}