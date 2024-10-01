#include <gtest/gtest.h>
#include <json/json.h>
#include <filesystem>
#include <fstream>
#include "server/backup/backup.h"
#include "server/restore/restore.h"

namespace fs = std::filesystem;

using namespace zipfiles::server;

class CommitLogTest : public ::testing::Test {
 protected:
  Json::Value sampleLog;
  Json::Value sampleLog2;
  Json::Value commitLogs;

  void SetUp() override {
    sampleLog["uuid"] = "1234";
    sampleLog["message"] = "Initial commit";

    sampleLog2["uuid"] = "5678";
    sampleLog2["message"] = "Second commit";

    commitLogs["data"] = Json::arrayValue;
    commitLogs["data"].append(sampleLog);
  }
};

TEST_F(CommitLogTest, IsCommittedTest) {
  EXPECT_TRUE(isCommitted(commitLogs, sampleLog));
  EXPECT_FALSE(isCommitted(commitLogs, sampleLog2));
}

TEST_F(CommitLogTest, AppendCommitLogTest) {
  appendCommitLog(commitLogs, sampleLog2);
  EXPECT_EQ(commitLogs["data"].size(), 2);
  EXPECT_EQ(commitLogs["data"][1]["uuid"].asString(), "5678");
}

TEST_F(CommitLogTest, WriteCommitLogTest) {
  fs::path testPath = "test_commit_log.json";
  writeCommitLog(testPath, commitLogs);

  std::ifstream logFile(testPath);
  ASSERT_TRUE(logFile.is_open());

  Json::Value readLogs;
  Json::CharReaderBuilder readerBuilder;
  std::string errs;
  ASSERT_TRUE(Json::parseFromStream(readerBuilder, logFile, &readLogs, &errs));
  logFile.close();

  EXPECT_EQ(readLogs["data"].size(), 1);
  EXPECT_EQ(readLogs["data"][0]["uuid"].asString(), "1234");

  fs::remove(testPath);  // Clean up
}

TEST_F(CommitLogTest, ReadCommitLogTest) {
  fs::path testPath = "test_commit_log.json";
  writeCommitLog(testPath, commitLogs);

  Json::Value readLogs = readCommitLog(testPath);
  EXPECT_EQ(readLogs["data"].size(), 1);
  EXPECT_EQ(readLogs["data"][0]["uuid"].asString(), "1234");

  fs::remove(testPath);  // Clean up
}

TEST_F(CommitLogTest, GetCommitLogByIdTest) {
  Json::Value log = getCommitLogById(commitLogs, "1234");
  EXPECT_EQ(log["uuid"].asString(), "1234");

  EXPECT_THROW(getCommitLogById(commitLogs, "9999"), std::runtime_error);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
