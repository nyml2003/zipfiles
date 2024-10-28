#ifndef ZIPFILES_MP_REQUEST_H
#define ZIPFILES_MP_REQUEST_H
#include <json/json.h>
#include <string>
#include <variant>
#include "json/value.h"
#include "mp/apis/GetCommitDetail.h"
#include "mp/apis/GetCommitList.h"
#include "mp/apis/GetFileDetail.h"
#include "mp/apis/GetFileDetailList.h"
#include "mp/apis/GetFileList.h"
#include "mp/apis/PostCommit.h"

namespace zipfiles {  // namespace request

namespace request {
struct MockNeedTime {
  int id;
};
}  // namespace request

struct Req;

using ReqKind = std::variant<
  request::GetCommitDetail,
  request::GetCommitList,
  request::GetFileList,
  request::GetFileDetail,
  request::GetFileDetailList,
  request::PostCommit,
  request::MockNeedTime>;

using ReqPtr = std::shared_ptr<Req>;

struct Req {
  explicit Req(ReqKind kind);
  ReqKind kind;
  double timestamp{};
  std::string uuid;
  Json::Value toJson();
  static ReqPtr fromJson(const Json::Value& json);
};

ReqPtr makeReqMockNeedTime(int id);
ReqPtr makeReqMockNeedTime(Json::Value payload);

}  // namespace zipfiles

#endif