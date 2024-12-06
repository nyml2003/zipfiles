#ifndef ZIPFILES_MP_RESPONSE_H
#define ZIPFILES_MP_RESPONSE_H

#include "mp/apis/CreateNullFolder.h"
#include "mp/apis/GetCommitDetail.h"
#include "mp/apis/GetCommitList.h"
#include "mp/apis/GetCommitRecycleBin.h"
#include "mp/apis/GetFileDetail.h"
#include "mp/apis/GetFileDetailList.h"
#include "mp/apis/GetFileList.h"
#include "mp/apis/LogicDeleteCommit.h"
#include "mp/apis/MockManyNotifications.h"
#include "mp/apis/MockNeedTime.h"
#include "mp/apis/PhysicalDeleteCommit.h"
#include "mp/apis/PostCommit.h"
#include "mp/apis/RecoverCommit.h"
#include "mp/apis/Restore.h"
#include "mp/common.h"

#include <json/json.h>
#include <variant>
namespace zipfiles {

namespace response {
struct NoResponse {
  std::string title;
  std::string description;
};
}  // namespace response

using ResKind = std::variant<
  response::GetFileList,
  response::GetFileDetailList,
  response::PostCommit,
  response::GetCommitDetail,
  response::GetCommitList,
  response::GetFileDetail,
  response::Restore,
  response::LogicDeleteCommit,
  response::PhysicalDeleteCommit,
  response::GetCommitRecycleBin,
  response::RecoverCommit,
  response::CreateNullFolder,
  response::MockNeedTime,
  response::MockManyNotifications,
  response::NoResponse>;

struct Res {
  Res(ResKind, std::string, Code);
  ResKind kind;      // 响应体
  Api api;           // 响应类型
  std::string uuid;  // 请求的uuid
  Code code;         // 响应状态码
  [[nodiscard]] Json::Value toJson() const;
  static Res fromJson(const Json::Value& json);
};

Api matchApi(ResKind);

}  // namespace zipfiles

#endif