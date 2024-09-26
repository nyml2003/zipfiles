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

ServerSocket::ServerSocket()
  : server_fd(socket(AF_INET, SOCK_STREAM, 0)), client_fd(0), address{} {
  if (server_fd == -1) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  addrlen = sizeof(address);

  if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
}

ServerSocket::~ServerSocket() {
  close(server_fd);
}

int ServerSocket::getServerFd() {
  return getInstance().server_fd;
}

void ServerSocket::acceptConnection() {
  getInstance().client_fd = accept(
    getInstance().server_fd,
    reinterpret_cast<struct sockaddr*>(&getInstance().address),
    reinterpret_cast<socklen_t*>(&getInstance().addrlen)
  );
  if (getInstance().client_fd < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
}

bool ServerSocket::receive(const RequestPtr& req) {
  std::array<char, MAX_MESSAGE_SIZE> buffer = {0};
  ssize_t valread =
    read(getInstance().client_fd, buffer.data(), MAX_MESSAGE_SIZE);
  if (valread == 0) {
    std::cout << "Client disconnected." << std::endl;
    close(getInstance().client_fd);
    return false;
  }
  if (valread < 0) {
    perror("Failed to receive request");
    return false;
  }
  std::cout << "Received data: " << buffer.data() << std::endl;
  static Json::CharReaderBuilder reader;
  Json::Value jsonData;
  std::string errs;
  std::istringstream stream(buffer.data());
  if (Json::parseFromStream(reader, stream, &jsonData, &errs)) {
    req->fromJson(jsonData);
    return true;
  }
  return false;
}

bool ServerSocket::send(const ResponsePtr& res) {
  static Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, res->toJson());
  std::cout << "Sending response: " << data << std::endl;
  ::send(getInstance().client_fd, data.c_str(), data.size(), 0);
  return true;
}

ClientSocket::ClientSocket()
  : sock(socket(AF_INET, SOCK_STREAM, 0)), serv_addr{} {
  if (sock < 0) {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    close(sock);
    exit(EXIT_FAILURE);
  }

  if (connect(sock, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
    perror("Connection Failed");
    close(sock);
    exit(EXIT_FAILURE);
  }
}

ClientSocket::~ClientSocket() {
  close(sock);
}

bool ClientSocket::send(const RequestPtr& req) {
  static Json::StreamWriterBuilder writer;
  std::string data = Json::writeString(writer, req->toJson());
  std::cout << "Sending request: " << data << std::endl;
  ::send(getInstance().sock, data.c_str(), data.size(), 0);
  return true;
}

bool ClientSocket::receive(const ResponsePtr& res) {
  std::array<char, MAX_MESSAGE_SIZE> buffer = {0};
  ssize_t valread = read(getInstance().sock, buffer.data(), MAX_MESSAGE_SIZE);
  std::cout << "Received data: " << buffer.data() << std::endl;
  if (valread > 0) {
    static Json::CharReaderBuilder reader;
    Json::Value jsonData;
    std::string errs;
    std::istringstream stream(buffer.data());
    if (Json::parseFromStream(reader, stream, &jsonData, &errs)) {
      res->fromJson(jsonData);
      return true;
    }
  }
  return false;
}

}  // namespace zipfiles::mp