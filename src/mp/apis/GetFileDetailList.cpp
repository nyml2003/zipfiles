#include "mp/apis/GetFileDetailList.h"

#include "log4cpp/Category.hh"

#include "mp/Request.h"

#include "mp/Response.h"

namespace zipfiles {

ReqPtr makeReqGetFileDetailList(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail list";
  return std::make_shared<Req>(request::GetFileDetailList{std::move(path)});
}

ReqPtr makeReqGetFileDetailList(
  std::string path,
  request::getFileDetailList::Filter filter
) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail list";
  return std::make_shared<Req>(
    request::GetFileDetailList{std::move(path), std::move(filter)}
  );
}

ReqPtr makeReqGetFileDetailList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILEDETAIL_LIST);
  return Req::fromJson(json);
}

ResPtr makeResGetFileDetailList(
  std::vector<response::getFileDetailList::FileDetail> files
) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file detail list";
  return std::make_shared<Res>(response::GetFileDetailList{std::move(files)});
}

ResPtr makeResGetFileDetailList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file detail list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILEDETAIL_LIST);
  return Res::fromJson(json);
}

}  // namespace zipfiles