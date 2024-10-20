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
#include <vector>
#include "json/value.h"
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
    fs::remove_all("/tmp/backup");
    fs::remove_all("/tmp/restore");
    fs::remove_all(log);

    // 创建测试目录和文件
    fs::create_directories("/tmp/test_dir1");
    fs::create_directories("/tmp/test_dir2");
    fs::create_directories("/tmp/test_dir3");

    // 创建普通文件
    std::ofstream regularFile("/tmp/test_dir1/regular_file.txt");
    regularFile << "This is a regular file." << std::endl;
    regularFile.close();

    // 创建文本文件
    std::ofstream textFile("/tmp/test_dir2/text_file.txt");
    textFile << "This is a text file." << std::endl;
    textFile.close();

    // 创建FIFO文件
    mkfifo("/tmp/test_dir3/fifo_file", 0666);

    // 创建Socket文件
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/test_dir3/socket_file");
    bind(sockfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    close(sockfd);

    // 创建设备文件
    mknod("/tmp/test_dir3/device_file", S_IFCHR | 0666, makedev(1, 7));

    // 创建一个100MB以上的文件，包含具体内容
    std::ofstream largeFile("/tmp/test_dir1/large_file.bin", std::ios::binary);
    std::vector<char> largeContent(100 * 1024 * 1024, 'A');  // 100MB of 'A'
    largeFile.write(largeContent.data(), largeContent.size());
    largeFile.close();

    // 创建软链接
    fs::create_symlink(
      "/tmp/test_dir1/regular_file.txt",
      "/tmp/test_dir1/symlink_to_regular_file"
    );

    // 创建硬链接
    fs::create_hard_link(
      "/tmp/test_dir1/regular_file.txt",
      "/tmp/test_dir1/hardlink_to_regular_file"
    );

    // 读出CommitTable到内存
    CommitTable::readCommitTable(COMMIT_TABLE_PATH);
  }

  void TearDown() override {
    // 删除测试目录和文件
    // fs::remove_all("/tmp/test_dir1");
    // fs::remove_all("/tmp/test_dir2");
    // fs::remove_all("/tmp/test_dir3");
    // fs::remove_all("/tmp/backup");
    // fs::remove_all("/tmp/restore");
    // fs::remove_all(log);
  }
};

TEST_F(BackupRestoreTest, BackupAndRestore) {
  // 备份文件
  std::vector<fs::path> files = {
    "/tmp/test_dir1/regular_file.txt",
    "/tmp/test_dir2/text_file.txt",
    "/tmp/test_dir3/fifo_file",
    "/tmp/test_dir3/socket_file",
    "/tmp/test_dir3/device_file",
    "/tmp/test_dir1/large_file.bin",
    "/tmp/test_dir1/symlink_to_regular_file",
    "/tmp/test_dir1/hardlink_to_regular_file"};

  CommitTableRecord cr;
  cr.message = "Test backup";
  cr.createTime = 1234567890.0;
  cr.uuid = "test-uuid";
  cr.storagePath = "/tmp/backup";
  cr.isEncrypt = true;

  std::string key = "test-key";

  // 调用备份函数
  ASSERT_NO_THROW(backupFiles(files, cr, key));

  // 恢复文件
  fs::path restorePath = "/tmp/restore";
  ASSERT_NO_THROW(restoreTo(restorePath, cr.uuid, key));

  // 使用cmp指令验证文件内容
  for (const auto& file : files) {
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
  // 假设有一个函数 getFileDetail 用于获取文件详细信息
  for (const auto& file : files) {
    if (fs::is_socket(file)) {
      continue;
    }
    fs::path relativePath = fs::relative(file.parent_path(), "/tmp");
    relativePath = relativePath / file.filename();
    FileDetail originalDetail = getFileDetail(file);
    FileDetail restoredDetail = getFileDetail(restorePath / relativePath);
    ASSERT_EQ(originalDetail.type, restoredDetail.type)
      << file << "&" << restorePath / relativePath;
    ASSERT_EQ(originalDetail.updateTime, restoredDetail.updateTime)
      << file << "&" << restorePath / relativePath;
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
}

}  // namespace zipfiles::server
