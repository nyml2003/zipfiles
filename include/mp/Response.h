#ifndef ZIPFILES_MP_RESPONSE_H
#define ZIPFILES_MP_RESPONSE_H
#include <json/json.h>
#include <variant>

#include "mp/apis/GetCommitDetail.h"
#include "mp/apis/GetCommitList.h"
#include "mp/apis/GetFileDetail.h"
#include "mp/apis/GetFileDetailList.h"
#include "mp/apis/GetFileList.h"
#include "mp/apis/MockManyNotifications.h"
#include "mp/apis/MockNeedTime.h"
#include "mp/apis/PostCommit.h"
#include "mp/common.h"

namespace zipfiles {

namespace response {
struct NoResponse {};
}  // namespace response

using ResKind = std::variant<
  response::GetFileList,
  response::GetFileDetailList,
  response::PostCommit,
  response::GetCommitDetail,
  response::GetCommitList,
  response::GetFileDetail,
  response::MockNeedTime,
  response::MockManyNotifications,
  response::NoResponse>;

struct Res {
  Res(
    ResKind,
    Api,
    std::string,
    Code,
    std::optional<std::string> = std::nullopt
  );
  ResKind kind;                        // 响应体
  Api api;                             // 响应类型
  std::string uuid;                    // 请求的uuid
  Code code;                           // 响应状态码
  std::optional<std::string> message;  // 响应消息, 可选，用于返回错误信息
  [[nodiscard]] Json::Value toJson() const;
  static Res fromJson(const Json::Value& json);
};

}  // namespace zipfiles

#endif