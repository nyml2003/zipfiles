#ifndef ZIPFILES_MP_RESPONSE_H
#define ZIPFILES_MP_RESPONSE_H
#include <json/json.h>
#include <variant>
#include "mp/apis/GetCommitDetail.h"
#include "mp/apis/GetCommitList.h"
#include "mp/apis/GetFileDetailList.h"
#include "mp/apis/GetFileList.h"
#include "mp/apis/PostCommit.h"

namespace zipfiles {

enum class StatusCode {
  UNKNOWN = 0,
  OK = 1,
  ERROR = 2,
};

enum class ApiEnum {
  IGNORE = 1,
  ERROR = 0,
  MOCK_NEED_TIME = 99,
  GET_FILE_LIST = 100,
  GET_FILEDETAIL_LIST = 101,
  POST_COMMIT = 102,
  GET_COMMIT_DETAIL = 103,
  GET_COMMIT_LIST = 104,
};

size_t toSizeT(ApiEnum apiEnum);

std::ostream& operator<<(std::ostream& os, const StatusCode& status);

namespace response {
struct MockNeedTime {
  int id;
};

}  // namespace response

struct Res;

using ResKind = std::variant<
  response::MockNeedTime,
  response::GetFileList,
  response::GetFileDetailList,
  response::PostCommit,
  response::GetCommitDetail,
  response::GetCommitList>;

using ResPtr = std::shared_ptr<Res>;

struct Res {
  explicit Res(ResKind kind);
  ResKind kind;
  double timestamp{};
  std::string uuid;
  StatusCode status{};
  Json::Value toJson();
  static ResPtr fromJson(const Json::Value& json);
};

ResPtr makeResMockNeedTime(int id);
ResPtr makeResMockNeedTime(Json::Value payload);

}  // namespace zipfiles

#endif