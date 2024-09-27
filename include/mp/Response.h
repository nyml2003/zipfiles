#ifndef ZIPFILES_MP_RESPONSE_H
#define ZIPFILES_MP_RESPONSE_H
#include "mp/dto.h"
#include <json/json.h>
#include <variant>
namespace zipfiles {
enum class StatusCode {
  UNKNOWN = 0,
  OK = 1,
  ERROR = 2,
};

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
  StatusCode status{};
  Json::Value toJson();
  static ResPtr fromJson(const Json::Value& json);
};

ResPtr makeResGetFileDetail(FileDetail metadata);

ResPtr makeResGetFileList(std::vector<File> files);

}  // namespace zipfiles

#endif