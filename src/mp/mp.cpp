#include "mp/mp.h"
#include <iostream>
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

MessageQueue::MessageQueue() {
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_MESSAGE_SIZE;
  attr.mq_curmsgs = 0;
  // 给this->name赋值"/zipfiles"
  this->name = new char[10];
  strcpy(this->name, "/zipfiles");

  mq = mq_open(this->name, O_CREAT | O_RDWR, 0666, &attr);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    throw std::runtime_error("Failed to create message queue");
  }
}

MessageQueue::~MessageQueue() {
  if (mq_close(mq) == -1) {
    perror("mq_close");
    std::cerr << "Failed to close message queue" << std::endl;
  }
  delete[] this->name;
}

bool MessageQueue::sendRequest(Request& req) {
  std::string jsonStr = req.toJson().toStyledString();
  if (mq_send(mq, jsonStr.c_str(), jsonStr.size() + 1, 0) == -1) {
    perror("mq_send");
    return false;
  }
  return true;
}

bool MessageQueue::receiveRequest(Request& req) {
  char buffer[MAX_MESSAGE_SIZE];
  if (mq_receive(mq, buffer, MAX_MESSAGE_SIZE, nullptr) == -1) {
    perror("mq_receive");
    return false;
  }

  Json::Value json;
  Json::Reader reader;
  if (!reader.parse(buffer, json)) {
    perror("mq_receive");
    return false;
  }
  req.fromJson(json);
  return true;
}

bool MessageQueue::sendResponse(Response& res) {
  std::string jsonStr = res.toJson().toStyledString();
  if (mq_send(mq, jsonStr.c_str(), jsonStr.size() + 1, 0) == -1) {
    perror("mq_send");
    printf("jsonStr: %ldKB\n", jsonStr.size() / 1024);
    return false;
  }
  return true;
}

bool MessageQueue::receiveResponse(Response& res) {
  char buffer[MAX_MESSAGE_SIZE];
  if (mq_receive(mq, buffer, MAX_MESSAGE_SIZE, nullptr) == -1) {
    perror("mq_receive");
    return false;
  }

  Json::Value json;
  Json::Reader reader;
  if (!reader.parse(buffer, json)) {
    perror("mq_receive");
    return false;
  }
  res.fromJson(json);
  return true;
}
}  // namespace zipfiles::mp