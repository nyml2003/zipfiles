#include "mp/Notification.h"

namespace zipfiles {

Notification::Notification(
  std::string message,
  Code code,
  std::optional<Json::Value> payload
)
  : message(std::move(message)), code(code), payload(std::move(payload)) {}

Json::Value Notification::toJson() const {
  Json::Value json;
  json["message"] = message;
  if (payload.has_value()) {
    json["payload"] = payload.value();
  }
  json["code"] = static_cast<int>(code);
  return std::move(json);
};

Notification Notification::fromJson(const Json::Value& json) {
  return Notification(
    json["message"].asString(), static_cast<Code>(json["code"].asInt()),
    json.isMember("payload") ? std::make_optional(json["payload"])
                             : std::nullopt
  );
}

}  // namespace zipfiles