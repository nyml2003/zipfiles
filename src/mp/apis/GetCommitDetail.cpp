#include "mp/apis/GetCommitDetail.h"
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/Response.h"

namespace zipfiles {

ReqPtr makeReqGetCommitDetail(std::string uuid) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get commit detail";
  return std::make_shared<Req>(request::GetCommitDetail{std::move(uuid)});
}

ReqPtr makeReqGetCommitDetail(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get commit detail";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_COMMIT_DETAIL);
  return Req::fromJson(json);
}

ResPtr makeResGetCommitDetail(
  std::vector<response::getCommitDetail::FileDetail> files
) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get commit detail";
  return std::make_shared<Res>(response::GetCommitDetail{std::move(files)});
}

ResPtr makeResGetCommitDetail(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get commit detail";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_COMMIT_DETAIL);
  return Res::fromJson(json);
}

}  // namespace zipfiles