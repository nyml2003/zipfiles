#include "mp/apis/GetFileDetail.h"

#include "log4cpp/Category.hh"

#include "mp/Request.h"

#include "mp/Response.h"

namespace zipfiles {

ReqPtr makeReqGetFileDetail(std::string path, std::string name) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail";
  return std::make_shared<Req>(
    request::GetFileDetail{std::move(path), std::move(name)}
  );
}

ReqPtr makeReqGetFileDetail(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file detail";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILEDETAIL_LIST);
  return Req::fromJson(json);
}

ResPtr makeResGetFileDetail(response::getFileDetail::FileDetail files) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file detail";
  return std::make_shared<Res>(response::GetFileDetail{std::move(files)});
}

ResPtr makeResGetFileDetail(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file detail";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILEDETAIL_LIST);
  return Res::fromJson(json);
}

}  // namespace zipfiles