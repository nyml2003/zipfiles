#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../tests/server/tools.h"
#include "server/backup/backup.h"
#include "server/configure/configure.h"
#include "server/restore/restore.h"
#include "server/tools/committable.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {
/**
 * @brief 测试单线程下的备份和恢复流程
 *
 */
class SingleThreadBackupAndRestore : public ::testing::Test {
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

TEST_F(SingleThreadBackupAndRestore, SingleThreadBackupAndRestore) {  // NOLINT
  // 切换到测试文件目录
  fs::path current_work_path = std::filesystem::current_path();
  current_work_path = current_work_path.parent_path() / "test_files";

  std::cout << "test_files path: " << current_work_path << std::endl;

  // test_files
  std::vector<fs::path> backup_files{};
  get_test_files(current_work_path, backup_files);

  for (const auto& path : backup_files) {
    std::cout << "Got file: " << path << std::endl;
  }

  // 创建一个cr
  CommitTableRecord cr;
  cr.message = "SingleThreadBackupAndRestore";
  cr.createTime = 1234567890.0;
  cr.uuid = "test-uuid";
  cr.storagePath = "/tmp/backup";
  cr.isEncrypt = true;

  std::string key = "test-key";

  // 调用备份函数
  // Start timing for backup
  auto start_backup = std::chrono::high_resolution_clock::now();

  ASSERT_NO_THROW(backupFiles(backup_files, cr, key));  // NOLINT

  std::cout << "Backup done" << std::endl;

  // End timing for backup and output the time taken
  auto end_backup = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> backup_time =
    end_backup - start_backup;
  std::cout << "Backup time: " << backup_time.count() << " ms" << std::endl;

  // 恢复文件
  fs::path restorePath = "/tmp/restore";

  auto start_restore = std::chrono::high_resolution_clock::now();

  ASSERT_NO_THROW(restoreTo(restorePath, cr.uuid, key));  // NOLINT

  std::cout << "Restore done" << std::endl;

  // End timing for restore and output the time taken
  auto end_restore = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> restore_time =
    end_restore - start_restore;
  std::cout << "Restore time: " << restore_time.count() << " ms" << std::endl;

  // 使用cmp指令验证文件内容
  for (const auto& file : backup_files) {
    fs::path relativePath = fs::relative(file, current_work_path);
    std::string originalFile = file.string();
    std::string restoredFile = (restorePath / relativePath).string();

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
  for (const auto& file : backup_files) {
    if (fs::is_socket(file)) {
      continue;
    }
    fs::path relativePath = fs::relative(file.parent_path(), current_work_path);
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
  }

  std::cout << "Compare done" << std::endl;

  uintmax_t origin_size = get_folder_size(current_work_path);
  uintmax_t compressed_size =
    get_folder_size(fs::path(cr.storagePath) / cr.uuid);

  // 输出压缩率
  std::cout << "Origin size: " << origin_size << std::endl;
  std::cout << "Compressed size: " << compressed_size << std::endl;
  std::cout << "Ratio: "
            << static_cast<double>(compressed_size) /
                 static_cast<double>(origin_size)
            << std::endl;
}

}  // namespace zipfiles::server
