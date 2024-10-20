#include "mp/apis/GetFileList.h"

#include "log4cpp/Category.hh"

#include "mp/Request.h"

#include "mp/Response.h"

namespace zipfiles {

ReqPtr makeReqGetFileList(std::string path) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file list";
  return std::make_shared<Req>(request::GetFileList{std::move(path)});
}

ReqPtr makeReqGetFileList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
  return Req::fromJson(json);
}
ResPtr makeResGetFileList(std::vector<response::getFileList::File> files) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file list";
  return std::make_shared<Res>(response::GetFileList{std::move(files)});
}

ResPtr makeResGetFileList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
  return Res::fromJson(json);
}

}  // namespace zipfiles