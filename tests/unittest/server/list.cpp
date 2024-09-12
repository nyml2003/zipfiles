#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "server/tools/fsapi.h"
#include "server/unittest_constant.h"

namespace fs = std::filesystem;
using namespace zipfiles::server;

class ListTest : public ::testing::Test {
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
    fs::create_directories(unittest::mockDir);
    fs::create_directories(unittest::userDir);

    fs::create_directories(unittest::userDir / "a");
    fs::create_directories(unittest::userDir / "b" / "c");
    std::ofstream(unittest::userDir / "a" / "a1.txt") << "这是 a1.txt";
    std::ofstream(unittest::userDir / "a" / "a2.txt") << "这是 a2.txt";
    std::ofstream(unittest::userDir / "b" / "b1.txt") << "这是 b1.txt";
    std::ofstream(unittest::userDir / "b" / "b2.txt") << "这是 b2.txt";
    std::ofstream(unittest::userDir / "b" / "c" / "c1.txt") << "这是 c1.txt";
    std::ofstream(unittest::userDir / "b" / "c" / "c2.txt") << "这是 c2.txt";
  }

  void TearDown() override {
    // 清理测试目录和文件
    fs::remove_all(unittest::mockDir);
  }
};

// TEST_F(ListTest, BackupFile) {
//   // 列出目录下的文件
//   auto files = list(unittest::userDir);
//   ASSERT_EQ(files.size(), 2);
//   ASSERT_EQ(files[0], "a");
//   ASSERT_EQ(files[1], "b");
// }
