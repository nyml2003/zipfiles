#include "mp/mp.h"
#include <arpa/inet.h>
#include <netinet/in.h>

namespace zipfiles::mp {

Json::Value Request::toJson() {
  Json::Value json;
  if (api.has_value()) {
    json["api"] = static_cast<int>(api.value());
  }
  if (payload.has_value()) {
    json["payload"] = payload.value();
  }
  return json;
}

void Request::fromJson(const Json::Value& json) {
  if (json.isMember("api")) {
    api = static_cast<ApiType>(json["api"].asInt());
  }
  if (json.isMember("payload")) {
    payload = json["payload"];
  }
}

bool Request::is(ApiType api) {
  return this->api.has_value() && this->api.value() == api;
}

void Request::setApi(ApiType api) {
  if (this->api.has_value()) {
    throw std::runtime_error("Api already set");
  }
  this->api = api;
}

Json::Value Request::getPayload() {
  if (!payload.has_value()) {
    throw std::runtime_error("Payload is empty");
  }
  return payload.value();
}

Json::Value Response::toJson() {
  Json::Value json;
  if (status.has_value()) {
    json["status"] = static_cast<int>(status.value());
  }
  if (payload.has_value()) {
    json["payload"] = payload.value();
  }
  return json;
}

void Response::fromJson(const Json::Value& json) {
  if (json.isMember("status")) {
    status = static_cast<StatusCode>(json["status"].asInt());
  }
  if (json.isMember("payload")) {
    payload = json["payload"];
  }
}

bool Response::is(StatusCode status) {
  return this->status.has_value() && this->status.value() == status;
}

Json::Value Response::getPayload() {
  if (!payload.has_value()) {
    throw std::runtime_error("Payload is empty");
  }
  return payload.value();
}

void Response::setStatus(StatusCode status) {
  this->status = status;
}

}  // namespace zipfiles::mp