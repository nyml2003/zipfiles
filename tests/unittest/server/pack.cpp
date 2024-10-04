#include "server/pack/pack.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "server/backup/backup.h"
#include "server/pack/unpack.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

// 辅助函数：创建一个包含指定内容的测试文件
void createTestFile(const fs::path& path, const std::string& content) {
  if (!fs::exists(path.parent_path())) {
    fs::create_directories(path.parent_path());
  }
  std::ofstream file(path);
  file << content;
  file.close();
}

// 测试夹具，用于设置和清理测试环境
class FilePackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 创建一些测试文件
    createTestFile("dir1/test1.txt", "Hello, World!");
    createTestFile("dir2/test2.txt", "Goodbye, World!");
  }

  void TearDown() override {
    // 清理测试文件
    fs::remove_all("dir1");
    fs::remove_all("dir2");
    fs::remove("test_write.txt");
  }
};

TEST_F(FilePackTest, ReadFile) {
  std::vector<uint8_t> data = readFile("dir1/test1.txt");
  std::string content(data.begin(), data.end());
  EXPECT_EQ(content, "Hello, World!");
}

TEST_F(FilePackTest, WriteFile) {
  std::vector<uint8_t> data = {'T', 'e', 's', 't'};
  writeFile("test_write.txt", data);

  std::ifstream file("test_write.txt", std::ios::binary);
  std::vector<uint8_t> readData(
    (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()
  );
  EXPECT_EQ(data, readData);
}

// TEST_F(FilePackTest, PackFiles) {
//   std::vector<fs::path> files = {"dir1/test1.txt", "dir2/test2.txt"};
//   std::vector<uint8_t> packedData = packFiles(files);

//   // 检查打包数据是否不为空
//   EXPECT_FALSE(packedData.empty());
// }

// TEST_F(FilePackTest, UnpackFiles) {
//   std::vector<fs::path> files = {"dir1/test1.txt", "dir2/test2.txt"};
//   std::vector<uint8_t> packedData = packFiles(files);

//   // 解包文件到临时目录
//   fs::path unpackDir = fs::temp_directory_path() / "unpack_files";
//   fs::create_directories(unpackDir);
//   unpackFiles(packedData, unpackDir);

//   // 验证解包后的文件内容
//   std::vector<uint8_t> file1Content = readFile(unpackDir / "dir1/test1.txt");
//   std::vector<uint8_t> file2Content = readFile(unpackDir / "dir2/test2.txt");

//   EXPECT_EQ(
//     std::string(file1Content.begin(), file1Content.end()), "Hello, World!"
//   );
//   EXPECT_EQ(
//     std::string(file2Content.begin(), file2Content.end()), "Goodbye, World!"
//   );

//   // 清理解包目录
//   fs::remove_all(unpackDir);
// }
//
}  // namespace zipfiles::server
