#include <log4cpp/convenience.h>
#include <log4cpp/Category.hh>
#include "mp/Notification.h"

namespace zipfiles {

Notification::Notification(NotificationKind kind, Code code)
  : kind(std::move(kind)), code(code) {}

bool isSingleLine(Code code) {
  return static_cast<int>(code) >= 400 && static_cast<int>(code) < 410;
}

bool isDoubleLine(Code code) {
  return static_cast<int>(code) >= 410 && static_cast<int>(code) < 420;
}

bool isBackup(Code code) {
  return static_cast<int>(code) >= 420 && static_cast<int>(code) < 430;
}

bool isRestore(Code code) {
  return static_cast<int>(code) >= 430 && static_cast<int>(code) < 440;
}

Json::Value Notification::toJson() const {
  Json::Value json;
  json["code"] = static_cast<int>(code);
  if (isSingleLine(code)) {
    json["payload"]["title"] = std::get<notification::SingleLine>(kind).title;
  } else if (isDoubleLine(code)) {
    auto dl = std::get<notification::DoubleLine>(kind);
    json["payload"]["title"] = dl.title;
    json["payload"]["description"] = dl.description;
  } else if (isBackup(code)) {
    json["payload"]["messageId"] =
      std::get<notification::Backup>(kind).messageId;
    json["payload"]["description"] =
      std::get<notification::Backup>(kind).description;
  } else if (isRestore(code)) {
    json["payload"]["messageId"] =
      std::get<notification::Restore>(kind).messageId;
    json["payload"]["description"] =
      std::get<notification::Restore>(kind).description;
  }
  return json;
};

Notification Notification::fromJson(const Json::Value& json) {
  auto code = static_cast<Code>(json["code"].asInt());
  NotificationKind kind;
  if (isSingleLine(code)) {
    kind =
      notification::SingleLine{.title = json["payload"]["title"].asString()};
  } else if (isDoubleLine(code)) {
    kind = notification::DoubleLine{
      .title = json["payload"]["title"].asString(),
      .description = json["payload"]["description"].asString()
    };
  } else if (isBackup(code)) {
    kind = notification::Backup{
      .messageId = json["payload"]["messageId"].asString(),
      .description = json["payload"]["description"].asString()
    };
  } else if (isRestore(code)) {
    kind = notification::Restore{
      .messageId = json["payload"]["messageId"].asString(),
      .description = json["payload"]["description"].asString()
    };
  }
  return {kind, code};
}

}  // namespace zipfiles