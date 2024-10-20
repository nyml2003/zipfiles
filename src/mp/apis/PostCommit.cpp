#include "mp/apis/PostCommit.h"

#include "log4cpp/Category.hh"

#include "mp/Request.h"

#include "mp/Response.h"

namespace zipfiles {

ReqPtr makeReqPostCommit(
  std::vector<std::string> files,
  std::string uuid,
  std::string message,
  double createTime,
  std::string storagePath,
  std::string author,
  bool isEncrypt,
  std::optional<std::string> key
) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to post commit";
  return std::make_shared<Req>(request::PostCommit{
    std::move(files), std::move(uuid), std::move(message), createTime,
    std::move(storagePath), std::move(author), isEncrypt, std::move(key)
  });
}

ReqPtr makeReqPostCommit(Json::Value payload) {
  log4cpp::Category::getRoot().infoStream()
    << "Making a request to post commit";
  Json::Value json;
  json["payload"] = std::move(payload);
  json["apiEnum"] = toSizeT(ApiEnum::POST_COMMIT);
  return Req::fromJson(json);
}

ResPtr makeResPostCommit() {
  log4cpp::Category::getRoot().infoStream()
    << "Making a response to post commit";
  return std::make_shared<Res>(response::PostCommit{});
}

}  // namespace zipfiles