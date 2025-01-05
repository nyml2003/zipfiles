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
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../tests/server/tools.h"
#include "server/backup/backup.h"
#include "server/configure/configure.h"
#include "server/restore/restore.h"
#include "server/tools/committable.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {
/**
 * @brief 测试两个线程同时备份不同的文件
 *
 */
class TwoThreadsDifferentFilesBackupAndRestore : public ::testing::Test {
 protected:
  void SetUp() override {
    // 清理文件
    fs::remove_all("/tmp/backup");
    fs::remove_all("/tmp/restore");
    fs::remove(COMMIT_TABLE_PATH);

    // 读出CommitTable到内存
    CommitTable::readCommitTable(COMMIT_TABLE_PATH);
  }

  void TearDown() override {
    // 删除测试目录和文件
    fs::remove_all("/tmp/backup");
    fs::remove_all("/tmp/restore");
    fs::remove(COMMIT_TABLE_PATH);
  }
};

void backup(
  const std::vector<fs::path>& files,
  const CommitTableRecord& cr,
  const std::string& key
) {
  CommitTableRecord crt = cr;

  ASSERT_NO_THROW(backupFiles(files, crt, key));  // NOLINT
}

void restore(
  const fs::path& dst,
  const std::string& uuid,
  const std::string& key
) {
  ASSERT_NO_THROW(restoreTo(dst, uuid, key));  // NOLINT
}

TEST_F(  // NOLINT
  TwoThreadsDifferentFilesBackupAndRestore,
  TwoThreadsDifferentFilesBackupAndRestore
) {
  // 切换到测试文件目录
  fs::path current_work_path = std::filesystem::current_path();
  current_work_path = current_work_path.parent_path() / "test_files";

  fs::path work_path_1 = current_work_path / "text";
  fs::path work_path_2 = current_work_path / "text";

  std::cout << "work_path_1: " << work_path_1 << std::endl;
  std::cout << "work_path_2: " << work_path_2 << std::endl;

  // test_files
  std::vector<fs::path> backup_files1{};
  get_test_files(work_path_1, backup_files1);

  std::vector<fs::path> backup_files2{};
  get_test_files(work_path_2, backup_files2);

  for (const auto& path : backup_files1) {
    std::cout << "Thread_1 got file: " << path << std::endl;
  }

  for (const auto& path : backup_files2) {
    std::cout << "Thread_2 got file: " << path << std::endl;
  }

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
      backupThreads.emplace_back(backup, backup_files1, cr1, key);
    } else {
      backupThreads.emplace_back(backup, backup_files2, cr2, key);
    }
  }

  // 等待备份完成
  for (auto& thread : backupThreads) {
    thread.join();
  }

  std::cout << "Backup done" << std::endl;

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

  std::cout << "Restore done" << std::endl;

  fs::path lca1 = getCommonAncestor(backup_files1);
  fs::path lca2 = getCommonAncestor(backup_files2);

  // 使用cmp指令验证文件内容
  for (const auto& file : backup_files1) {
    fs::path relativePath = fs::relative(file.parent_path(), lca1);
    std::string originalFile = file.string();
    std::string restoredFile = restorePath / relativePath / file.filename();

    if (fs::is_fifo(file) || fs::is_block_file(file) || fs::is_character_file(file) || fs::is_socket(file)) {
      continue;
    }

    std::string cmpCommand =
      "cmp --silent " + originalFile + " " + restoredFile;  // NOLINT
    ASSERT_EQ(system(cmpCommand.c_str()), 0)                // NOLINT
      << "文件 " << originalFile << " 和 " << restoredFile << " 不同";
  }

  // 使用cmp指令验证文件内容
  for (const auto& file : backup_files2) {
    fs::path relativePath = fs::relative(file.parent_path(), lca2);
    std::string originalFile = file.string();
    std::string restoredFile = restorePath / relativePath / file.filename();

    if (fs::is_fifo(file) || fs::is_block_file(file) || fs::is_character_file(file) || fs::is_socket(file)) {
      continue;
    }

    std::string cmpCommand =
      "cmp --silent " + originalFile + " " + restoredFile;  // NOLINT
    ASSERT_EQ(system(cmpCommand.c_str()), 0)                // NOLINT
      << "文件 " << originalFile << " 和 " << restoredFile << " 不同";
  }

  // 验证FileDetail完全一致
  // 使用fsapi来获取文件信息
  for (const auto& file : backup_files1) {
    if (fs::is_socket(file)) {
      continue;
    }
    fs::path relativePath = fs::relative(file.parent_path(), lca1);
    std::string originalFile = file.string();
    std::string restoredFile = restorePath / relativePath / file.filename();

    FileDetail originalDetail = getFileDetail(originalFile);
    FileDetail restoredDetail = getFileDetail(restoredFile);
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
  }

  // 验证FileDetail完全一致
  // 使用fsapi来获取文件信息
  for (const auto& file : backup_files2) {
    if (fs::is_socket(file)) {
      continue;
    }
    fs::path relativePath = fs::relative(file.parent_path(), lca2);
    std::string originalFile = file.string();
    std::string restoredFile = restorePath / relativePath / file.filename();

    FileDetail originalDetail = getFileDetail(originalFile);
    FileDetail restoredDetail = getFileDetail(restoredFile);
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
  }

  std::cout << "Compare done" << std::endl;

  uintmax_t origin_size_1 = get_folder_size(work_path_1);
  uintmax_t compressed_size_1 =
    get_folder_size(fs::path(cr1.storagePath) / cr1.uuid);

  // 输出压缩率
  std::cout << "Thread_1 origin size: " << origin_size_1 << std::endl;
  std::cout << "Thread_1 compressed size: " << compressed_size_1 << std::endl;
  std::cout << "Thread_1 ratio: "
            << static_cast<double>(compressed_size_1) /
                 static_cast<double>(origin_size_1)
            << std::endl;

  uintmax_t origin_size_2 = get_folder_size(work_path_2);
  uintmax_t compressed_size_2 =
    get_folder_size(fs::path(cr2.storagePath) / cr2.uuid);

  // 输出压缩率
  std::cout << "Thread_2 origin size: " << origin_size_2 << std::endl;
  std::cout << "Thread_2 compressed size: " << compressed_size_2 << std::endl;
  std::cout << "Thread_2 ratio: "
            << static_cast<double>(compressed_size_2) /
                 static_cast<double>(origin_size_2)
            << std::endl;
}

}  // namespace zipfiles::server
