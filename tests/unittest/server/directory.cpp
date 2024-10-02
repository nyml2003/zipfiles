#include <gtest/gtest.h>
#include <json/json.h>
#include <filesystem>
#include <fstream>
#include "mp/dto.h"
#include "server/backup/backup.h"
#include "server/restore/restore.h"

using namespace zipfiles::server;
using namespace zipfiles;

namespace fs = std::filesystem;

// Helper function to create a sample JSON file
void createSampleJsonFile(const fs::path& path, const Json::Value& content) {
  std::ofstream outFile(path);
  Json::StreamWriterBuilder writer;
  outFile << Json::writeString(writer, content);
  outFile.close();
}

// Helper function to create test files
void createTestFiles() {
  std::ofstream file1("/app/file1.txt");
  file1 << "This is the content of file1.";
  file1.close();

  std::ofstream file2("/app/file2.txt");
  file2 << "This is the content of file2.";
  file2.close();
}

void removeTestFiles() {
  fs::remove("/app/file1.txt");
  fs::remove("/app/file2.txt");
}

// Test readDirectoryFile function
TEST(DirectoryFileTest, ReadDirectoryFile) {
  fs::path testFile = "/app/test_read.json";
  Json::Value sampleContent;
  sampleContent["data"] = Json::arrayValue;
  sampleContent["data"].append("sample");

  createSampleJsonFile(testFile, sampleContent);

  Json::Value result = readDirectoryFile(testFile);
  EXPECT_EQ(result, sampleContent);

  fs::remove(testFile);  // Clean up
}

// Test writeDirectoryFile function
TEST(DirectoryFileTest, WriteDirectoryFile) {
  createTestFiles();

  fs::path testFile = "/app/test_read.json";
  std::vector<fs::path> files = {"app/file1.txt", "app/file2.txt"};

  writeDirectoryFile(testFile, files);

  std::ifstream inFile(testFile);
  Json::Value result;
  inFile >> result;

  EXPECT_EQ(result["data"].size(), files.size());

  fs::remove(testFile);  // Clean up
  removeTestFiles();     // Clean up
}

// Combined test for readDirectoryFile and writeDirectoryFile
TEST(DirectoryFileTest, ReadWriteDirectoryFile) {
  createTestFiles();

  fs::path testFile = "/app/test_combined.json";
  std::vector<fs::path> files = {"app/file1.txt", "app/file2.txt"};

  writeDirectoryFile(testFile, files);
  Json::Value readResult = readDirectoryFile(testFile);

  EXPECT_EQ(readResult["data"].size(), files.size());

  fs::remove(testFile);  // Clean up
  removeTestFiles();     // Clean up
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
