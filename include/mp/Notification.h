#ifndef ZIPFILES_MP_NOTIFICATION_H
#define ZIPFILES_MP_NOTIFICATION_H

#include <json/json.h>
#include <string>
#include <variant>

#include "mp/common.h"

namespace zipfiles {

namespace notification {

struct SingleLine {
  std::string title;
};

struct DoubleLine {
  std::string title;
  std::string description;
};

struct Backup {
  std::string messageId;
  std::string description;
};

struct Restore {
  std::string messageId;
  std::string description;
};

struct NoneNotification {};

}  // namespace notification

using NotificationKind = std::variant<
  notification::SingleLine,
  notification::DoubleLine,
  notification::Backup,
  notification::Restore,
  notification::NoneNotification>;

struct Notification {
  Notification(NotificationKind kind, Code code);
  NotificationKind kind;  // 通知体
  Code code;              // 状态码
  [[nodiscard]] Json::Value toJson() const;
  static Notification fromJson(const Json::Value& json);
};

bool isSingleLine(Code code);

bool isDoubleLine(Code code);

bool isBackup(Code code);

bool isRestore(Code code);

}  // namespace zipfiles
using ZNotification = zipfiles::Notification;
#endif