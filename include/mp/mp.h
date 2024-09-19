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
  Jsonable() = default;
  virtual ~Jsonable() = default;
  Jsonable(const Jsonable& other) = delete;
  Jsonable& operator=(const Jsonable& other) = delete;
  Jsonable(Jsonable&& other) noexcept = delete;
  Jsonable& operator=(Jsonable&& other) noexcept = delete;
  virtual Json::Value toJson() = 0;
  virtual void fromJson(const Json::Value& json) = 0;
};

using JsonablePtr = std::shared_ptr<Jsonable>;

class Request : public Jsonable {
 public:
  explicit Request() = default;
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  bool is(ApiType api);
  void setApi(ApiType api);
  Json::Value getPayload();
  template <
    typename T,
    typename = std::enable_if_t<
      std::is_base_of<Jsonable, T>::value ||
      std::is_base_of<Jsonable, typename T::element_type>::value>>
  void setPayload(T payload) {
    static_assert(
      std::is_base_of<Jsonable, T>::value ||
        std::is_base_of<Jsonable, typename T::element_type>::value,
      "Payload must be derived from Jsonable or a shared_ptr to a type derived "
      "from Jsonable"
    );
    if (this->payload.has_value()) {
      throw std::runtime_error("Payload already set");
    }
    if constexpr (std::is_base_of<Jsonable, T>::value) {
      this->payload = payload.toJson();
    } else if constexpr (std::is_base_of<
                           Jsonable, typename T::element_type>::value) {
      this->payload = payload->toJson();
    }
  }

 private:
  std::optional<ApiType> api;
  std::optional<Json::Value> payload;
};

using RequestPtr = std::shared_ptr<Request>;

class Response : public Jsonable {
 public:
  explicit Response() = default;
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  bool is(StatusCode status);
  template <
    typename T,
    typename = std::enable_if_t<
      std::is_base_of<Jsonable, T>::value ||
      std::is_base_of<Jsonable, typename T::element_type>::value>>
  void setPayload(T payload) {
    static_assert(
      std::is_base_of<Jsonable, T>::value ||
        std::is_base_of<Jsonable, typename T::element_type>::value,
      "Payload must be derived from Jsonable or a shared_ptr to a type derived "
      "from Jsonable"
    );
    if (this->payload.has_value()) {
      throw std::runtime_error("Payload already set");
    }
    if constexpr (std::is_base_of<Jsonable, T>::value) {
      this->payload = payload.toJson();
    } else if constexpr (std::is_base_of<
                           Jsonable, typename T::element_type>::value) {
      this->payload = payload->toJson();
    }
  }
  Json::Value getPayload();
  void setStatus(StatusCode status);

 private:
  std::optional<StatusCode> status;
  std::optional<Json::Value> payload;
};

using ResponsePtr = std::shared_ptr<Response>;

class ServerSocket {
 public:
  static ServerSocket& getInstance() {
    try {
      static ServerSocket instance;
      return instance;
    } catch (const std::exception& e) {
      throw std::runtime_error(e.what());
    }
  }
  [[nodiscard]] bool receive(const RequestPtr& req) const;
  [[nodiscard]] bool send(const ResponsePtr& res) const;
  void acceptConnection();
  ServerSocket(const ServerSocket& other) = delete;
  ServerSocket& operator=(const ServerSocket& other) = delete;
  ServerSocket(ServerSocket&& other) noexcept = delete;
  ServerSocket& operator=(ServerSocket&& other) noexcept = delete;

 private:
  ServerSocket();
  ~ServerSocket();
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen;
};

class ClientSocket {
 public:
  static ClientSocket& getInstance() {
    try {
      static ClientSocket instance;
      return instance;
    } catch (const std::exception& e) {
      throw std::runtime_error(e.what());
    }
  }
  [[nodiscard]] bool receive(const ResponsePtr& res) const;
  [[nodiscard]] bool send(const RequestPtr& req) const;
  ClientSocket(const ClientSocket& other) = delete;
  ClientSocket& operator=(const ClientSocket& other) = delete;
  ClientSocket(ClientSocket&& other) noexcept = delete;
  ClientSocket& operator=(ClientSocket&& other) noexcept = delete;

 private:
  ClientSocket();
  ~ClientSocket();
  int sock;
  struct sockaddr_in serv_addr;
};

}  // namespace zipfiles::mp
#endif  // ! ZIPFILES_MQ_TYPE_H