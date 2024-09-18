#include "mp/mp.h"
#include <arpa/inet.h>
#include <netinet/in.h>
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

ServerSocket::ServerSocket() {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
  addrlen = sizeof(address);

  if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}

ServerSocket::~ServerSocket() {
  close(server_fd);
}

void ServerSocket::acceptConnection() {
  client_fd =
    accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
  if (client_fd < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
}

bool ServerSocket::receive(Request& req) {
  char buffer[MAX_MESSAGE_SIZE] = {0};
  int valread = read(client_fd, buffer, MAX_MESSAGE_SIZE);
  if (valread == 0) {
    std::cout << "Client disconnected." << std::endl;
    close(client_fd);
    return false;
  } else if (valread < 0) {
    perror("Failed to receive request");
    return false;
  }
  std::cout << "Received data: " << buffer << std::endl;
  Json::CharReaderBuilder reader;
  Json::Value jsonData;
  std::string errs;
  std::istringstream s(buffer);
  if (Json::parseFromStream(reader, s, &jsonData, &errs)) {
    req.fromJson(jsonData);
    return true;
  }
  return false;
}

bool ServerSocket::send(Response& res) {
  Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, res.toJson());
  std::cout << "Sending response: " << data << std::endl;
  ::send(client_fd, data.c_str(), data.size(), 0);
  return true;
}

ClientSocket::ClientSocket() {
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection Failed");
    exit(EXIT_FAILURE);
  }
}

ClientSocket::~ClientSocket() {
  close(sock);
}

bool ClientSocket::send(Request& req) {
  Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, req.toJson());
  std::cout << "Sending request: " << data << std::endl;
  ::send(sock, data.c_str(), data.size(), 0);
  return true;
}

bool ClientSocket::receive(Response& res) {
  char buffer[MAX_MESSAGE_SIZE] = {0};
  int valread = read(sock, buffer, MAX_MESSAGE_SIZE);
  std::cout << "Received data: " << buffer << std::endl;
  if (valread > 0) {
    Json::CharReaderBuilder reader;
    Json::Value jsonData;
    std::string errs;
    std::istringstream s(buffer);
    if (Json::parseFromStream(reader, s, &jsonData, &errs)) {
      res.fromJson(jsonData);
      return true;
    }
  }
  return false;
}

}  // namespace zipfiles::mp