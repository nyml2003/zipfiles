#include "mp/apis/GetCommitList.h"

#include "log4cpp/Category.hh"

#include "mp/Request.h"

#include "mp/Response.h"

namespace zipfiles {

ReqPtr makeReqGetCommitList() {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get commit list";
  return std::make_shared<Req>(request::GetCommitList{});
}

ResPtr makeResGetCommitList(
  std::vector<response::getCommitList::CommitLog> commits
) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get commit list";
  return std::make_shared<Res>(response::GetCommitList{std::move(commits)});
}

ResPtr makeResGetCommitList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get commit list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_COMMIT_LIST);
  return Res::fromJson(json);
}

}  // namespace zipfiles