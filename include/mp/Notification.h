#ifndef ZIPFILES_MP_RESPONSE_H
#define ZIPFILES_MP_RESPONSE_H

#include <json/json.h>
#include <optional>
#include <string>

#include "mp/common.h"

namespace zipfiles {

struct Notification {
  Notification(std::string message, std::optional<Json::Value> payload);
  std::string message;  // 消息内容
  std::optional<Json::Value> payload;
  Code code{Code::NOTIFICATION};  // 状态码
  [[nodiscard]] Json::Value toJson() const;
  static Notification fromJson(const Json::Value& json);
};

}  // namespace zipfiles

#endif