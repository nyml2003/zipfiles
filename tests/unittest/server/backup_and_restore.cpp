#include <gtest/gtest.h>
#include <json/json.h>
#include <openssl/sha.h>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include "server/backup/backup.h"
#include "server/restore/restore.h"

namespace fs = std::filesystem;

namespace zipfiles::server {
// 测试类
class BackupRestoreTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 创建测试目录和文件
    testDir = fs::temp_directory_path() / "backup_restore_test";
    fs::create_directories(testDir);

    testFile1 = testDir / "test1.txt";
    testFile2 = testDir / "test2.txt";

    std::ofstream(testFile1) << "This is a test file 1.";
    std::ofstream(testFile2) << "This is a test file 2.";

    files = {testFile1, testFile2};

    // 创建commit log
    cl["message"] = "Test commit";
    cl["createTime"] = "2024-10-06T19:20:18";
    cl["uuid"] = "1234-5678-91011";
    cl["storagePath"] = (testDir / "backup.zip").string();
    cl["isEncrypt"] = false;  // 不加密
  }

  void TearDown() override {
    // 清理测试目录
    fs::remove_all(testDir);
    // 移除commit log文件
    fs::path commitLogPath =
      fs::path(std::getenv("HOME")) / ".zip" / "commit.log";
    fs::remove(commitLogPath);
  }

  fs::path testDir;
  fs::path testFile1;
  fs::path testFile2;
  std::vector<fs::path> files;
  Json::Value cl;
  std::string key = "testkey";  // 加密密钥
};

TEST_F(BackupRestoreTest, BackupAndRestore) {
  // 备份文件
  ASSERT_NO_THROW(backupFiles(files, cl, key));

  // 确认备份文件存在
  fs::path backupFile = cl["storagePath"].asString();
  ASSERT_TRUE(fs::exists(backupFile));

  // 创建恢复目录
  fs::path restoreDir = testDir / "restore";
  fs::create_directories(restoreDir);

  // 恢复文件
  ASSERT_NO_THROW(restoreTo(restoreDir, cl["uuid"].asString(), key));

  // 确认恢复的文件存在并且内容正确
  fs::path restoredFile1 = restoreDir / "test1.txt";
  fs::path restoredFile2 = restoreDir / "test2.txt";

  ASSERT_TRUE(fs::exists(restoredFile1));
  ASSERT_TRUE(fs::exists(restoredFile2));

  std::ifstream restoredFile1Stream(restoredFile1);
  std::ifstream restoredFile2Stream(restoredFile2);

  std::string restoredFile1Content(
    (std::istreambuf_iterator<char>(restoredFile1Stream)),
    std::istreambuf_iterator<char>()
  );
  std::string restoredFile2Content(
    (std::istreambuf_iterator<char>(restoredFile2Stream)),
    std::istreambuf_iterator<char>()
  );

  ASSERT_EQ(restoredFile1Content, "This is a test file 1.");
  ASSERT_EQ(restoredFile2Content, "This is a test file 2.");
}

// TEST_F(BackupRestoreTest, EncryptedBackupAndRestore) {
//   // 启用加密
//   cl["isEncrypt"] = true;

//   // 使用加密备份文件
//   ASSERT_NO_THROW(backupFiles(files, cl, key));

//   // 确认备份文件存在
//   fs::path backupFile = cl["storagePath"].asString();
//   ASSERT_TRUE(fs::exists(backupFile));

//   // 创建恢复目录
//   fs::path restoreDir = testDir / "restore_encrypted";
//   fs::create_directories(restoreDir);

//   // 使用加密恢复文件
//   ASSERT_NO_THROW(restoreTo(restoreDir, cl["uuid"].asString(), key));

//   // 确认恢复的文件存在并且内容正确
//   fs::path restoredFile1 = restoreDir / "test1.txt";
//   fs::path restoredFile2 = restoreDir / "test2.txt";

//   ASSERT_TRUE(fs::exists(restoredFile1));
//   ASSERT_TRUE(fs::exists(restoredFile2));

//   std::ifstream restoredFile1Stream(restoredFile1);
//   std::ifstream restoredFile2Stream(restoredFile2);

//   std::string restoredFile1Content(
//     (std::istreambuf_iterator<char>(restoredFile1Stream)),
//     std::istreambuf_iterator<char>()
//   );
//   std::string restoredFile2Content(
//     (std::istreambuf_iterator<char>(restoredFile2Stream)),
//     std::istreambuf_iterator<char>()
//   );

//   ASSERT_EQ(restoredFile1Content, "This is a test file 1.");
//   ASSERT_EQ(restoredFile2Content, "This is a test file 2.");
// }

TEST_F(BackupRestoreTest, BackupAndRestoreDifferentDirectories) {
  // 创建额外的目录和文件
  fs::path subDir = testDir / "subdir";
  fs::create_directories(subDir);
  fs::path testFile3 = subDir / "test3.txt";
  std::ofstream(testFile3) << "This is a test file 3.";
  files.push_back(testFile3);

  // 备份文件
  ASSERT_NO_THROW(backupFiles(files, cl, key));

  // 确认备份文件存在
  fs::path backupFile = cl["storagePath"].asString();
  ASSERT_TRUE(fs::exists(backupFile));

  // 创建恢复目录
  fs::path restoreDir = testDir / "restore_different_dirs";
  fs::create_directories(restoreDir);

  // 恢复文件
  ASSERT_NO_THROW(restoreTo(restoreDir, cl["uuid"].asString(), key));

  // 确认恢复的文件存在并且内容正确
  fs::path restoredFile1 = restoreDir / "test1.txt";
  fs::path restoredFile2 = restoreDir / "test2.txt";
  fs::path restoredFile3 = restoreDir / "subdir" / "test3.txt";

  ASSERT_TRUE(fs::exists(restoredFile1));
  ASSERT_TRUE(fs::exists(restoredFile2));
  ASSERT_TRUE(fs::exists(restoredFile3));

  std::ifstream restoredFile1Stream(restoredFile1);
  std::ifstream restoredFile2Stream(restoredFile2);
  std::ifstream restoredFile3Stream(restoredFile3);

  std::string restoredFile1Content(
    (std::istreambuf_iterator<char>(restoredFile1Stream)),
    std::istreambuf_iterator<char>()
  );
  std::string restoredFile2Content(
    (std::istreambuf_iterator<char>(restoredFile2Stream)),
    std::istreambuf_iterator<char>()
  );
  std::string restoredFile3Content(
    (std::istreambuf_iterator<char>(restoredFile3Stream)),
    std::istreambuf_iterator<char>()
  );

  ASSERT_EQ(restoredFile1Content, "This is a test file 1.");
  ASSERT_EQ(restoredFile2Content, "This is a test file 2.");
  ASSERT_EQ(restoredFile3Content, "This is a test file 3.");
}

TEST_F(BackupRestoreTest, BackupAndRestoreLargeFiles) {
  // 创建一个大测试文件
  fs::path largeTestFile = testDir / "large_test.txt";
  std::ofstream ofs(largeTestFile);
  // 2000000 -> 4MB
  // 50000000 -> 100MB
  for (int i = 0; i < 50000000; ++i) {
    ofs << "A ";
  }
  ofs.close();
  files.push_back(largeTestFile);

  // 备份文件
  ASSERT_NO_THROW(backupFiles(files, cl, key));

  // 确认备份文件存在
  fs::path backupFile = cl["storagePath"].asString();
  ASSERT_TRUE(fs::exists(backupFile));

  // 创建恢复目录
  fs::path restoreDir = testDir / "restore_large_files";
  fs::create_directories(restoreDir);

  // 恢复文件
  ASSERT_NO_THROW(restoreTo(restoreDir, cl["uuid"].asString(), key));

  // 确认恢复的文件存在并且内容正确
  fs::path restoredFile1 = restoreDir / "test1.txt";
  fs::path restoredFile2 = restoreDir / "test2.txt";
  fs::path restoredLargeFile = restoreDir / "large_test.txt";

  ASSERT_TRUE(fs::exists(restoredFile1));
  ASSERT_TRUE(fs::exists(restoredFile2));
  ASSERT_TRUE(fs::exists(restoredLargeFile));

  // 使用cmp命令比较文件
  std::string cmpCommand =
    "cmp -s " + largeTestFile.string() + " " + restoredLargeFile.string();
  int result = std::system(cmpCommand.c_str());

  ASSERT_EQ(result, 0);
}

// TEST_F(BackupRestoreTest, EncryptedMultipleDirectoriesBackupAndRestore) {
//   // 创建多个目录和文件
//   fs::path dir1 = testDir / "dir1";
//   fs::path dir2 = testDir / "dir2";
//   fs::create_directories(dir1);
//   fs::create_directories(dir2);

//   fs::path file1 = dir1 / "file1.txt";
//   fs::path file2 = dir2 / "file2.txt";

//   std::ofstream(file1) << "This is file 1 in dir1.";
//   std::ofstream(file2) << "This is file 2 in dir2.";

//   files = {file1, file2};

//   // 启用加密
//   cl["isEncrypt"] = true;

//   // 使用加密备份文件
//   ASSERT_NO_THROW(backupFiles(files, cl, key));

//   // 确认备份文件存在
//   fs::path backupFile = cl["storagePath"].asString();
//   ASSERT_TRUE(fs::exists(backupFile));

//   // 创建恢复目录
//   fs::path restoreDir = testDir / "restore_multiple_dirs";
//   fs::create_directories(restoreDir);

//   // 使用加密恢复文件
//   ASSERT_NO_THROW(restoreTo(restoreDir, cl["uuid"].asString(), key));

//   // 确认恢复的文件存在并且内容正确
//   fs::path restoredFile1 = restoreDir / "dir1" / "file1.txt";
//   fs::path restoredFile2 = restoreDir / "dir2" / "file2.txt";

//   ASSERT_TRUE(fs::exists(restoredFile1));
//   ASSERT_TRUE(fs::exists(restoredFile2));

//   std::ifstream restoredFile1Stream(restoredFile1);
//   std::ifstream restoredFile2Stream(restoredFile2);

//   std::string restoredFile1Content(
//     (std::istreambuf_iterator<char>(restoredFile1Stream)),
//     std::istreambuf_iterator<char>()
//   );
//   std::string restoredFile2Content(
//     (std::istreambuf_iterator<char>(restoredFile2Stream)),
//     std::istreambuf_iterator<char>()
//   );

//   ASSERT_EQ(restoredFile1Content, "This is file 1 in dir1.");
//   ASSERT_EQ(restoredFile2Content, "This is file 2 in dir2.");
// }

// TEST_F(BackupRestoreTest, EncryptedLargeFileBackupAndRestore) {
//   // 创建一个大文件
//   fs::path largeFile = testDir / "large_test_file.txt";
//   std::ofstream largeFileStream(largeFile);
//   largeFileStream << std::string(
//     static_cast<size_t>(1024 * 1024 * 2), 'A'
//   );  // 2 MB 文件
//   largeFileStream.close();

//   files.push_back(largeFile);

//   // 启用加密
//   cl["isEncrypt"] = true;

//   // 使用加密备份文件
//   ASSERT_NO_THROW(backupFiles(files, cl, key));

//   // 确认备份文件存在
//   fs::path backupFile = cl["storagePath"].asString();
//   ASSERT_TRUE(fs::exists(backupFile));

//   // 创建恢复目录
//   fs::path restoreDir = testDir / "restore_large_encrypted";
//   fs::create_directories(restoreDir);

//   // 使用加密恢复文件
//   ASSERT_NO_THROW(restoreTo(restoreDir, cl["uuid"].asString(), key));

//   // 确认恢复的文件存在并且内容正确
//   fs::path restoredLargeFile = restoreDir / "large_test_file.txt";
//   ASSERT_TRUE(fs::exists(restoredLargeFile));

//   // 使用cmp比较原始文件和恢复文件
//   std::string cmpCommand =
//     "cmp -s " + largeFile.string() + " " + restoredLargeFile.string();
//   int cmpResult = std::system(cmpCommand.c_str());

//   ASSERT_EQ(cmpResult, 0);
// }

}  // namespace zipfiles::server
