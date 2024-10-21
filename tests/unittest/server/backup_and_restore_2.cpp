#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "server/backup/backup.h"
#include "server/configure/configure.h"
#include "server/restore/restore.h"
#include "server/tools/committable.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {

class BackupRestoreTest : public ::testing::Test {
 protected:
  fs::path log = "/usr/local/zipfiles/.zip";

  void SetUp() override {
    // 清理文件
    fs::remove_all("/tmp/test_dir1");
    fs::remove_all("/tmp/test_dir2");
    fs::remove_all("/tmp/test_dir3");
    fs::remove_all("/tmp/test_dir4");
    fs::remove_all("/tmp/test_dir5");
    fs::remove_all("/tmp/test_dir6");
    fs::remove_all("/tmp/backup");
    fs::remove_all("/tmp/restore");
    fs::remove_all(log);

    // 创建测试目录和文件
    fs::create_directories("/tmp/test_dir1");
    fs::create_directories("/tmp/test_dir2");
    fs::create_directories("/tmp/test_dir3");
    fs::create_directories("/tmp/test_dir4");
    fs::create_directories("/tmp/test_dir5");
    fs::create_directories("/tmp/test_dir6");

    // 创建普通文件1
    std::ofstream regularFile1("/tmp/test_dir1/regular_file1.txt");
    regularFile1 << "This is a regular file1." << std::endl;
    regularFile1.close();

    // 创建普通文件2
    std::ofstream regularFile2("/tmp/test_dir4/regular_file2.txt");
    regularFile2 << "This is a regular file2." << std::endl;
    regularFile2.close();

    // 创建文本文件1
    std::ofstream textFile1("/tmp/test_dir2/text_file1.txt");
    textFile1 << "This is a text file1." << std::endl;
    textFile1.close();

    // 创建文本文件2
    std::ofstream textFile2("/tmp/test_dir5/text_file2.txt");
    textFile2 << "This is a text file2." << std::endl;
    textFile2.close();

    // 创建FIFO文件
    mkfifo("/tmp/test_dir3/fifo_file1", 0666);
    mkfifo("/tmp/test_dir6/fifo_file2", 0666);

    // 创建Socket文件
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/test_dir3/socket_file");
    bind(sockfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    close(sockfd);

    // 创建设备文件
    mknod("/tmp/test_dir3/device_file1", S_IFCHR | 0666, makedev(1, 7));

    // 创建设备文件
    mknod("/tmp/test_dir6/device_file2", S_IFCHR | 0666, makedev(2, 8));

    // 大文件1
    std::ofstream largeFile1(
      "/tmp/test_dir1/large_file1.bin", std::ios::binary
    );
    std::vector<char> largeContent1(100 * 1024 * 1024, 'A');  // 100MB of 'A'
    largeFile1.write(largeContent1.data(), largeContent1.size());
    largeFile1.close();

    // 大文件2
    std::ofstream largeFile2(
      "/tmp/test_dir4/large_file2.bin", std::ios::binary
    );
    std::vector<char> largeContent2(100 * 1024 * 1024, 'B');  // 100MB of 'B'
    largeFile2.write(largeContent2.data(), largeContent2.size());
    largeFile2.close();

    // 创建软链接1
    fs::create_symlink(
      "/tmp/test_dir1/regular_file1.txt",
      "/tmp/test_dir1/symlink_to_regular_file1"
    );

    // 创建软链接2
    fs::create_symlink(
      "/tmp/test_dir4/regular_file2.txt",
      "/tmp/test_dir4/symlink_to_regular_file2"
    );

    // 创建硬链接1
    fs::create_hard_link(
      "/tmp/test_dir1/regular_file1.txt",
      "/tmp/test_dir1/hardlink_to_regular_file1"
    );

    // 创建硬链接2
    fs::create_hard_link(
      "/tmp/test_dir4/regular_file2.txt",
      "/tmp/test_dir4/hardlink_to_regular_file2"
    );

    // 读出CommitTable到内存
    CommitTable::readCommitTable(COMMIT_TABLE_PATH);
  }

  void TearDown() override {
    // 删除测试目录和文件
    // fs::remove_all("/tmp/test_dir1");
    // fs::remove_all("/tmp/test_dir2");
    // fs::remove_all("/tmp/test_dir3");
    // fs::remove_all("/tmp/test_dir4");
    // fs::remove_all("/tmp/test_dir5");
    // fs::remove_all("/tmp/test_dir6");
    // fs::remove_all("/tmp/backup");
    // fs::remove_all("/tmp/restore");
    // fs::remove_all(log);
  }
};

void backup(
  const std::vector<fs::path>& files,
  const CommitTableRecord& cr,
  const std::string& key
) {
  ASSERT_NO_THROW(backupFiles(files, cr, key));
}

void restore(
  const fs::path& dst,
  const std::string& uuid,
  const std::string& key
) {
  ASSERT_NO_THROW(restoreTo(dst, uuid, key));
}

TEST_F(BackupRestoreTest, ConcurrentBackupAndRestoreDifferentFiles) {
  // 备份文件
  std::vector<fs::path> files1 = {
    "/tmp/test_dir1/regular_file1.txt",
    "/tmp/test_dir1/symlink_to_regular_file1",
    "/tmp/test_dir1/hardlink_to_regular_file1",
    "/tmp/test_dir1/large_file1.bin",
    "/tmp/test_dir2/text_file1.txt",
    "/tmp/test_dir3/fifo_file1",
    "/tmp/test_dir3/socket_file",
    "/tmp/test_dir3/device_file1"};

  std::vector<fs::path> files2 = {
    "/tmp/test_dir4/regular_file2.txt",
    "/tmp/test_dir4/symlink_to_regular_file2",
    "/tmp/test_dir4/hardlink_to_regular_file2",
    "/tmp/test_dir4/large_file2.bin",
    "/tmp/test_dir5/text_file2.txt",
    "/tmp/test_dir6/fifo_file2",
    "/tmp/test_dir6/device_file2"};

  CommitTableRecord cr1;
  cr1.message = "This is cr1";
  cr1.createTime = 1234567890.0;
  cr1.uuid = "cr1";
  cr1.storagePath = "/tmp/backup";
  cr1.isEncrypt = true;
  cr1.isDelete = false;

  CommitTableRecord cr2;
  cr2.message = "This is cr2";
  cr2.createTime = 1234567890.0;
  cr2.uuid = "cr2";
  cr2.storagePath = "/tmp/backup";
  cr2.isEncrypt = true;
  cr2.isDelete = false;

  std::string key = "test-key";

  std::vector<std::thread> backupThreads;

  // 备份
  for (int i = 0; i < 2; i++) {
    if (!i) {
      backupThreads.emplace_back(backup, files1, cr1, key);
    } else {
      backupThreads.emplace_back(backup, files2, cr2, key);
    }
  }

  // 等待备份完成
  for (auto& thread : backupThreads) {
    thread.join();
  }

  // 恢复文件
  fs::path restorePath = "/tmp/restore";

  std::vector<std::thread> restoreThreads;

  for (int i = 0; i < 2; i++) {
    if (!i) {
      restoreThreads.emplace_back(restore, restorePath, cr1.uuid, key);
    } else {
      restoreThreads.emplace_back(restore, restorePath, cr2.uuid, key);
    }
  }

  // 等待恢复完成
  for (auto& thread : restoreThreads) {
    thread.join();
  }

  // 使用cmp指令验证文件内容
  for (const auto& file : files1) {
    fs::path relativePath = fs::relative(file, "/tmp");
    std::string originalFile = file.string();
    std::string restoredFile = (restorePath / relativePath).string();

    if (fs::is_fifo(file) || fs::is_block_file(file) || fs::is_character_file(file) || fs::is_socket(file)) {
      continue;
    }

    std::string cmpCommand =
      "cmp --silent " + originalFile + " " + restoredFile;
    ASSERT_EQ(system(cmpCommand.c_str()), 0)
      << "Files " << originalFile << " and " << restoredFile << " differ.";
  }

  for (const auto& file : files2) {
    fs::path relativePath = fs::relative(file, "/tmp");
    std::string originalFile = file.string();
    std::string restoredFile = (restorePath / relativePath).string();

    if (fs::is_fifo(file) || fs::is_block_file(file) || fs::is_character_file(file) || fs::is_socket(file)) {
      continue;
    }

    std::string cmpCommand =
      "cmp --silent " + originalFile + " " + restoredFile;
    ASSERT_EQ(system(cmpCommand.c_str()), 0)
      << "Files " << originalFile << " and " << restoredFile << " differ.";
  }

  // 验证FileDetail完全一致
  for (const auto& file : files1) {
    if (fs::is_socket(file)) {
      continue;
    }
    fs::path relativePath = fs::relative(file.parent_path(), "/tmp");
    relativePath = relativePath / file.filename();
    FileDetail originalDetail = getFileDetail(file);
    FileDetail restoredDetail = getFileDetail(restorePath / relativePath);
    ASSERT_EQ(originalDetail.type, restoredDetail.type)
      << file << "&" << restorePath / relativePath;
    if (!fs::is_character_file(file) && !fs::is_block_file(file)) {
      ASSERT_EQ(originalDetail.updateTime, restoredDetail.updateTime)
        << file << " & " << restorePath / relativePath;
    }
    ASSERT_EQ(originalDetail.size, restoredDetail.size)
      << file << "&" << restorePath / relativePath;
    ASSERT_EQ(originalDetail.owner, restoredDetail.owner)
      << file << "&" << restorePath / relativePath;
    ASSERT_EQ(originalDetail.group, restoredDetail.group)
      << file << "&" << restorePath / relativePath;
    ASSERT_EQ(originalDetail.mode, restoredDetail.mode)
      << file << "&" << restorePath / relativePath;
    ASSERT_EQ(originalDetail.dev, restoredDetail.dev)
      << file << "&" << restorePath / relativePath;
  }

  for (const auto& file : files2) {
    if (fs::is_socket(file)) {
      continue;
    }
    fs::path relativePath = fs::relative(file.parent_path(), "/tmp");
    relativePath = relativePath / file.filename();
    FileDetail originalDetail = getFileDetail(file);
    FileDetail restoredDetail = getFileDetail(restorePath / relativePath);
    ASSERT_EQ(originalDetail.type, restoredDetail.type)
      << file << " & " << restorePath / relativePath;
    if (!fs::is_character_file(file) && !fs::is_block_file(file)) {
      ASSERT_EQ(originalDetail.updateTime, restoredDetail.updateTime)
        << file << " & " << restorePath / relativePath;
    }
    ASSERT_EQ(originalDetail.size, restoredDetail.size)
      << file << " & " << restorePath / relativePath;
    ASSERT_EQ(originalDetail.owner, restoredDetail.owner)
      << file << " & " << restorePath / relativePath;
    ASSERT_EQ(originalDetail.group, restoredDetail.group)
      << file << " & " << restorePath / relativePath;
    ASSERT_EQ(originalDetail.mode, restoredDetail.mode)
      << file << " & " << restorePath / relativePath;
    ASSERT_EQ(originalDetail.dev, restoredDetail.dev)
      << file << " & " << restorePath / relativePath;
  }
}

}  // namespace zipfiles::server
