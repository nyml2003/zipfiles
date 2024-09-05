#include <iostream>
#include "unittest_constant.h"
#include <filesystem>
#include <fstream>
#include "backup.h"
namespace fs = std::filesystem;
using namespace zipfiles;
const fs::path demoDir = "/app/tests/demo";
const fs::path backupDir = demoDir / "backup";
const fs::path userDir = demoDir / "user";
int main(int argc, char** argv) {
  //   fs::create_directories(demoDir);
  //   fs::create_directories(userDir);

  //   fs::create_directories(userDir / "a");
  //   fs::create_directories(userDir / "b" / "c");
  //   std::ofstream(userDir / "a" / "a1.txt") << "这是 a1.txt";
  //   std::ofstream(userDir / "a" / "a2.txt") << "这是 a2.txt";
  //   std::ofstream(userDir / "b" / "b1.txt") << "这是 b1.txt";
  //   std::ofstream(userDir / "b" / "b2.txt") << "这是 b2.txt";
  //   std::ofstream(userDir / "b" / "c" / "c1.txt") << "这是 c1.txt";
  //   std::ofstream(userDir / "b" / "c" / "c2.txt") << "这是 c2.txt";
  //   backup(userDir, backupDir);
  // restore(backupDir, userDir);
  return 0;
}