#include <mp/Request.h>
#include <mp/Response.h>
#include <server/backup/backup.h>
#include <server/socket/api.h>
#include <vector>
#include "json/value.h"
#include "log4cpp/Category.hh"
#include "server/configure/configure.h"

namespace zipfiles::server::api {
template <>
response::GetCommitList handle<
  request::GetCommitList,
  response::GetCommitList>(const request::GetCommitList& /* unused */) {
  log4cpp::Category::getRoot().infoStream() << "start GetCommitList";
  // 读取COMMIT_TABLE_PATH的内容到Json::Value, 用std::ifstream
  // std::ifstream commitTableFile(COMMIT_TABLE_PATH);
  // Json::Value commitTable;
  // commitTableFile >> commitTable;
  // commitTableFile.close();
  Json::Value commitTable = CommitTable::readCommitTableView(COMMIT_TABLE_PATH);

  response::GetCommitList response;

  for (const auto& uuid : commitTable.getMemberNames()) {
    Json::Value commit = commitTable[uuid];
    response.commits.push_back({
      .uuid = uuid,
      .message = commit["message"].asString(),
      .createTime = commit["createTime"].asDouble(),
      .storagePath = commit["storagePath"].asString(),
      .author = commit["author"].asString(),
      .isEncrypt = commit["isEncrypt"].asBool(),
      .isDelete = commit["isDelete"].asBool(),
    });
  }

  return response;
}

}  // namespace zipfiles::server::api