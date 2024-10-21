#include "mp/apis/GetFileDetailList.h"

#include "log4cpp/Category.hh"

#include "mp/Request.h"

#include "mp/Response.h"

namespace zipfiles {

ReqPtr makeReqGetFileList(
  std::string path,
  std::optional<request::getFileDetailList::Filter> filter
) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to get file list";
  return std::make_shared<Req>(
    request::GetFileDetailList{std::move(path), std::move(filter)}
  );
}

// ReqPtr makeReqGetFileList(Json::Value payload) {
//   log4cpp::Category::getRoot().infoStream()
//     << "Making a request to get file list";
//   Json::Value json;
//   json["payload"] = std::move(payload);
//   json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
//   return Req::fromJson(json);
// }
// ResPtr makeResGetFileDetailList(
//   std::vector<response::getFileDetailList::FileDetail> files
// ) {
//   log4cpp::Category::getRoot().infoStream()
//     << "Making a response to get file list";
//   return
//   std::make_shared<Res>(response::GetFileDetailList{std::move(files)});
// }

ResPtr makeResGetFileDetailList(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to get file list";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::GET_FILE_LIST);
  return Res::fromJson(json);
}

}  // namespace zipfiles