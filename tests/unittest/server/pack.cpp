#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "server/pack/pack.h"

namespace fs = std::filesystem;

// 测试 createArchive 函数
TEST(ArchiveTest, CreateArchive) {
  std::string archiveName = "test_archive.bin";
  std::ofstream archive = zipfiles::server::createArchive(archiveName);

  ASSERT_TRUE(archive.is_open());
  archive.close();

  // 清理测试文件
  fs::remove(archiveName);
}

// 测试 packFileToArchive 函数
TEST(ArchiveTest, PackFileToArchive) {
  std::string archiveName = "test_archive.bin";
  std::string testFileName = "test_file.txt";
  std::string fileContent = "This is a test file.";

  // 创建测试文件
  std::ofstream testFile(testFileName);
  testFile << fileContent;
  testFile.close();

  // 创建归档文件
  std::ofstream archive = zipfiles::server::createArchive(archiveName);

  // 打包文件到归档
  ASSERT_NO_THROW(zipfiles::server::packFileToArchive(archive, testFileName));
  archive.close();

  // 验证归档文件内容
  std::ifstream archiveIn(archiveName, std::ios::binary);
  ASSERT_TRUE(archiveIn.is_open());

  size_t pathLength;
  archiveIn.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
  ASSERT_EQ(pathLength, testFileName.size());

  std::string pathStr(pathLength, '\0');
  archiveIn.read(pathStr.data(), pathLength);
  ASSERT_EQ(pathStr, testFileName);

  size_t fileSize;
  archiveIn.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
  ASSERT_EQ(fileSize, fileContent.size());

  std::string readContent(fileSize, '\0');
  archiveIn.read(readContent.data(), fileSize);
  ASSERT_EQ(readContent, fileContent);

  archiveIn.close();

  // 清理测试文件
  fs::remove(testFileName);
  fs::remove(archiveName);
}
