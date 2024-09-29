#ifndef ZIPFILES_MP_RESPONSE_H
#define ZIPFILES_MP_RESPONSE_H
#include <json/json.h>
#include <variant>
#include "mp/dto.h"

namespace zipfiles {

enum class StatusCode {
  UNKNOWN = 0,
  OK = 1,
  ERROR = 2,
};

enum class ApiEnum {
  GET_FILE_DETAIL = 0,
  GET_FILE_LIST = 1,
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

}  // namespace response

struct Res;

using ResKind = std::variant<response::GetFileDetail, response::GetFileList>;
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

}  // namespace zipfiles

#endif