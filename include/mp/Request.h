#ifndef ZIPFILES_MP_REQUEST_H
#define ZIPFILES_MP_REQUEST_H
#include <json/json.h>
#include <string>
#include <variant>

#include "mp/apis/GetCommitDetail.h"
#include "mp/apis/GetCommitList.h"
#include "mp/apis/GetFileDetail.h"
#include "mp/apis/GetFileDetailList.h"
#include "mp/apis/GetFileList.h"
#include "mp/apis/MockNeedTime.h"
#include "mp/apis/PostCommit.h"
#include "mp/common.h"

namespace zipfiles {  // namespace request

using ReqKind = std::variant<
  request::GetCommitDetail,
  request::GetCommitList,
  request::GetFileList,
  request::GetFileDetail,
  request::GetFileDetailList,
  request::PostCommit,
  request::MockNeedTime>;

struct Req {
  Req(ReqKind, Api, std::string);
  ReqKind kind;      // 请求体
  Api api;           // 请求类型
  std::string uuid;  // 请求的uuid
  [[nodiscard]] Json::Value toJson() const;
  static Req fromJson(const Json::Value&);
};

}  // namespace zipfiles

#endif