#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "server/pack/pack.h"
#include "server/pack/unpack.h"

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

TEST(UnpackArchiveTest, UnpacksMultipleFilesCorrectly) {
  std::string archivePath = "test_archive.bin";
  std::string outputDir = "test_output";

  // 创建一个模拟的归档文件
  std::ofstream archive(archivePath, std::ios::binary);
  ASSERT_TRUE(archive) << "Failed to create archive: " << archivePath;

  // 写入多个文件到归档文件
  std::vector<std::pair<std::string, std::string>> files = {
    {"file1.txt", "Hello, World!"},
    {"file2.txt", "Goodbye, World!"},
    {"file3.txt", "Hello again!"}};

  for (const auto& [filePath, fileContent] : files) {
    size_t pathLength = filePath.size();
    size_t fileSize = fileContent.size();

    archive.write(
      reinterpret_cast<const char*>(&pathLength), sizeof(pathLength)
    );
    archive.write(filePath.data(), pathLength);
    archive.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
    archive.write(fileContent.data(), fileSize);
  }
  archive.close();

  // 调用函数解压归档文件
  zipfiles::server::unpackArchive(archivePath, outputDir);

  // 验证输出目录和文件
  for (const auto& [filePath, fileContent] : files) {
    fs::path outputPath = fs::path(outputDir) / filePath;
    ASSERT_TRUE(fs::exists(outputPath));

    // 验证解压文件的内容
    std::ifstream extractedFile(outputPath, std::ios::binary);
    ASSERT_TRUE(extractedFile);

    std::string extractedContent(
      (std::istreambuf_iterator<char>(extractedFile)),
      std::istreambuf_iterator<char>()
    );
    EXPECT_EQ(extractedContent, fileContent);
  }

  // 清理
  fs::remove(archivePath);
  fs::remove_all(outputDir);
}

TEST(ArchiveIntegrationTest, PackAndUnpackMultipleFiles) {
  std::string archiveName = "integration_test_archive.bin";
  std::string outputDir = "integration_test_output";

  // 要打包的文件
  std::vector<std::pair<std::string, std::string>> files = {
    {"file1.txt", "Hello, World!"},
    {"file2.txt", "Goodbye, World!"},
    {"file3.txt", "Hello again!"}};

  // 创建并打包文件到归档
  std::ofstream archive = zipfiles::server::createArchive(archiveName);
  ASSERT_TRUE(archive.is_open());

  for (const auto& [filePath, fileContent] : files) {
    // 创建测试文件
    std::ofstream testFile(filePath);
    testFile << fileContent;
    testFile.close();

    // 打包文件到归档
    ASSERT_NO_THROW(zipfiles::server::packFileToArchive(archive, filePath));
  }
  archive.close();

  // 解压归档文件
  zipfiles::server::unpackArchive(archiveName, outputDir);

  // 验证解压后的文件
  for (const auto& [filePath, fileContent] : files) {
    fs::path outputPath = fs::path(outputDir) / filePath;
    ASSERT_TRUE(fs::exists(outputPath));

    // 验证解压文件的内容
    std::ifstream extractedFile(outputPath, std::ios::binary);
    ASSERT_TRUE(extractedFile);

    std::string extractedContent(
      (std::istreambuf_iterator<char>(extractedFile)),
      std::istreambuf_iterator<char>()
    );
    EXPECT_EQ(extractedContent, fileContent);

    // 清理测试文件
    fs::remove(filePath);
  }

  // 清理归档文件和输出目录
  fs::remove(archiveName);
  fs::remove_all(outputDir);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}