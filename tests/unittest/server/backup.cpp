#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "server/unittest_constant.h"
namespace server = zipfiles::server;
class BackupRestoreTest : public ::testing::Test {
 protected:
  void SetUp() override {
    /*
    |-- a
    |   |-- a1.txt
    |   `-- a2.txt
    `-- b
        |-- b1.txt
        `-- b2.txt
        `-- c
            |-- c1.txt
            `-- c2.txt
    */
    fs::create_directories(server::unittest::mockDir);
    fs::create_directories(server::unittest::userDir);

    fs::create_directories(server::unittest::userDir / "a");
    fs::create_directories(server::unittest::userDir / "b" / "c");
    std::ofstream(server::unittest::userDir / "a" / "a1.txt") << "这是 a1.txt";
    std::ofstream(server::unittest::userDir / "a" / "a2.txt") << "这是 a2.txt";
    std::ofstream(server::unittest::userDir / "b" / "b1.txt") << "这是 b1.txt";
    std::ofstream(server::unittest::userDir / "b" / "b2.txt") << "这是 b2.txt";
    std::ofstream(server::unittest::userDir / "b" / "c" / "c1.txt")
      << "这是 c1.txt";
    std::ofstream(server::unittest::userDir / "b" / "c" / "c2.txt")
      << "这是 c2.txt";
  }

  void TearDown() override {
    // 清理测试目录和文件
    fs::remove_all(server::unittest::mockDir);
  }
};
