#ifndef ZIPFILES_MP_RESPONSE_H
#define ZIPFILES_MP_RESPONSE_H
#include <json/json.h>
#include <variant>
#include "mp/Request.h"
#include "mp/dto.h"

namespace zipfiles {

enum class StatusCode {
  UNKNOWN = 0,
  OK = 1,
  ERROR = 2,
};

enum class ApiEnum {
  IGNORE = 1,
  ERROR = 0,
  GET_FILE_DETAIL = 100,
  GET_FILE_LIST = 101,
  POST_COMMIT = 102,
};

size_t toSizeT(ApiEnum apiEnum);

std::ostream& operator<<(std::ostream& os, const StatusCode& status);

namespace response {

struct GetFileDetail {
  FileDetail metadata;
};

struct GetFileList {
  std::vector<File> files;
};
// TODO 测试耗时请求
struct MockNeedTime {
  int id;
};

}  // namespace response

struct Res;

using ResKind = std::variant<
  response::GetFileDetail,
  response::GetFileList,
  response::MockNeedTime>;
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

ResPtr makeResGetFileDetail(FileDetail metadata);

ResPtr makeResGetFileList(std::vector<File> files);

ResPtr makeResMockNeedTime(int id);

}  // namespace zipfiles

#endif