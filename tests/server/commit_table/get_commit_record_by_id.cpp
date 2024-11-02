#include <gtest/gtest.h>
#include <exception>
#include <filesystem>
#include <iostream>
#include "server/configure/configure.h"
#include "server/tools/committable.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

class GetCommitRecordById : public ::testing::Test {
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

TEST_F(GetCommitRecordById, GetCommitRecordById) {  // NOLINT
  Json::Value root;

  // 此时硬盘上应该有一个空commit_table
  ASSERT_NO_THROW(  // NOLINT
    root = CommitTable::readCommitTableView(COMMIT_TABLE_PATH)
  );

  std::cout << "Empty commit_table: " << root.toStyledString() << std::endl;

  // 创建两个commit record
  CommitTableRecord cr1 = {"cr1",         "test_message",    123456.0,
                           "/tmp/backup", "test_author",     false,
                           false,         "test_encoded_key"};

  CommitTableRecord cr2 = {"cr2",         "test_message",    123456.0,
                           "/tmp/backup", "test_author",     false,
                           false,         "test_encoded_key"};

  // 加入cr1
  ASSERT_FALSE(CommitTable::isCommitted(cr1));

  std::cout << "Append record_1 in memory: "
            << CommitTable::toJson(cr1).toStyledString() << std::endl;

  std::cout << "Try to get cr1" << std::endl;

  // 尝试获取cr1
  ASSERT_NO_THROW(CommitTable::getCommitRecordById("cr1"));  // NOLINT

  std::cout << "Success" << std::endl;

  std::cout << "Try to get cr2" << std::endl;

  // 尝试获取cr2
  ASSERT_THROW(  // NOLINT
    CommitTable::getCommitRecordById("cr2"), std::exception
  );

  std::cout << "Failed" << std::endl;

  // 加入cr2
  ASSERT_FALSE(CommitTable::isCommitted(cr2));

  std::cout << "Append record_2 in memory: "
            << CommitTable::toJson(cr1).toStyledString() << std::endl;

  std::cout << "Try to get cr2" << std::endl;

  // 尝试获取cr2
  ASSERT_NO_THROW(CommitTable::getCommitRecordById("cr2"));  // NOLINT

  std::cout << "Success" << std::endl;
}

}  // namespace zipfiles::server