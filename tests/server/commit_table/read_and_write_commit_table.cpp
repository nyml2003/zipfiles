#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>
#include "server/configure/configure.h"
#include "server/tools/committable.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

class ReadAndWriteCommitTable : public ::testing::Test {
 protected:
  void SetUp() override {
    // 清理文件
    fs::remove(COMMIT_TABLE_PATH);

    // 读出CommitTable到内存
    // 不存在时会创建一个新的
    CommitTable::readCommitTable(COMMIT_TABLE_PATH);
  }

  void TearDown() override {
    // 删除测试目录和文件
    fs::remove(COMMIT_TABLE_PATH);
  }
};

TEST_F(ReadAndWriteCommitTable, ReadAndWriteCommitTable) {  // NOLINT
  Json::Value root;

  // 此时硬盘上应该有一个空commit_table
  ASSERT_NO_THROW(  // NOLINT
    root = CommitTable::readCommitTableView(COMMIT_TABLE_PATH)
  );

  std::cout << "Empty commit_table: " << root.toStyledString() << std::endl;

  ASSERT_TRUE(root.empty());

  CommitTableRecord cr = {"test_uuid",   "test_message",    123456.0,
                          "/tmp/backup", "test_author",     false,
                          false,         "test_encoded_key"};

  // 往内存中的commit_table新增一个commit
  ASSERT_FALSE(CommitTable::isCommitted(cr));

  std::cout << "Append record in memory: "
            << CommitTable::toJson(cr).toStyledString() << std::endl;

  // 再次读取
  ASSERT_NO_THROW(  // NOLINT
    root = CommitTable::readCommitTableView(COMMIT_TABLE_PATH)
  );

  std::cout << "commit_table after append: " << root.toStyledString()
            << std::endl;

  ASSERT_TRUE(root.empty());

  // 写硬盘
  ASSERT_NO_THROW(CommitTable::writeCommitTable(COMMIT_TABLE_PATH));  // NOLINT

  // 再次读取
  ASSERT_NO_THROW(  // NOLINT
    root = CommitTable::readCommitTableView(COMMIT_TABLE_PATH)
  );

  std::cout << "commit_table after write: " << root.toStyledString()
            << std::endl;

  ASSERT_FALSE(root.empty());
}

}  // namespace zipfiles::server