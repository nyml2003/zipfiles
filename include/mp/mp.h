#ifndef ZIPFILES_MQ_TYPE_H
#define ZIPFILES_MQ_TYPE_H
#include <json/json.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <optional>
#include <cstddef>
namespace zipfiles::mp {
constexpr size_t MAX_MESSAGE_SIZE = (1 << 16);

enum class ApiType { GET_FILE_LIST = 0, GET_FILE };

enum class StatusCode { OK = 0, ERROR };

class Jsonable {
 public:
  virtual Json::Value toJson() = 0;
  virtual void fromJson(const Json::Value& json) = 0;
};

class Request : public Jsonable {
 public:
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  bool is(ApiType api);
  void setApi(ApiType api);
  Json::Value getPayload();
  template <
    typename T,
    typename = std::enable_if_t<std::is_base_of<Jsonable, T>::value>>
  void setPayload(T payload) {
    static_assert(
      std::is_base_of<Jsonable, T>::value,
      "Payload must be derived from Jsonable"
    );
    if (this->payload.has_value()) {
      throw std::runtime_error("Payload already set");
    }
    this->payload = payload.toJson();
  }
  

 private:
  std::optional<ApiType> api;
  std::optional<Json::Value> payload;
};

class Response : public Jsonable {
 public:
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  bool is(StatusCode status);
  template <
    typename T,
    typename = std::enable_if_t<std::is_base_of<Jsonable, T>::value>>
  void setPayload(T payload) {
    static_assert(
      std::is_base_of<Jsonable, T>::value,
      "Payload must be derived from Jsonable"
    );
    if (this->payload.has_value()) {
      throw std::runtime_error("Payload already set");
    }
    this->payload = payload.toJson();
  }
  Json::Value getPayload();
  void setStatus(StatusCode status);

 private:
  std::optional<StatusCode> status;
  std::optional<Json::Value> payload;
};

class ServerSocket {
 public:
  ServerSocket();
  ~ServerSocket();
  bool receive(Request& req);
  bool send(Response& res);
  void acceptConnection();

 private:
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen;
};

class ClientSocket {
 public:
  ClientSocket();
  ~ClientSocket();
  bool send(Request& req);
  bool receive(Response& res);

 private:
  int sock;
  struct sockaddr_in serv_addr;
};

}  // namespace zipfiles::mp
#endif  // ! ZIPFILES_MQ_TYPE_H