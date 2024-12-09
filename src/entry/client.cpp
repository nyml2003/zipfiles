
#include <fstream>
#include "client/launcher.h"
using zipfiles::client::Launcher;
int main(int argc, char* argv[]) {

  Launcher::startLogger();

  Launcher::startGTK(argc, argv);
  Launcher::getInstance().startReciever();
  gtk_main();
  Launcher::getInstance().reciever.join();
  return 0;
}