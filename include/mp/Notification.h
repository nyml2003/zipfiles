#ifndef ZIPFILES_MP_NOTIFICATION_H
#define ZIPFILES_MP_NOTIFICATION_H

#include <json/json.h>
#include <optional>
#include <string>

#include "mp/common.h"

namespace zipfiles {

struct Notification {
  explicit Notification(
    std::string message,
    Code code,
    std::optional<Json::Value> payload = std::nullopt
  );
  std::string message;  // 消息内容
  std::optional<Json::Value> payload;
  Code code;  // 状态码
  [[nodiscard]] Json::Value toJson() const;
  static Notification fromJson(const Json::Value& json);
};

}  // namespace zipfiles

#endif