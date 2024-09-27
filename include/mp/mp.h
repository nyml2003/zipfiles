#ifndef ZIPFILES_MQ_TYPE_H
#define ZIPFILES_MQ_TYPE_H
#include <json/json.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <optional>
#include <cstddef>
namespace zipfiles::mp {
/**
 * @brief 最大消息长度= 2^16B = 64KB
 */
constexpr size_t MAX_MESSAGE_SIZE = (1 << 16);
/**
 * @brief 服务器端口
 */
constexpr size_t PORT = 8080;
/**
 * @brief API枚举
 */
enum class ApiType { GET_FILE_LIST = 0, GET_FILE_DETAIL };

/**
 * @brief 状态码
 */
enum class StatusCode { OK = 0, ERROR };

/**
 * @brief Jsonable接口
 * @details 用于序列化和反序列化
 * 继承该抽象类的类可以和Json::Value互相转换
 * 从而可以通过转化为字符串，再转化为二进制流进行传输
 */
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

/**
 * @brief 请求DTO
 * @details
 * 请求数据传输对象，包含API类型和请求数据
 */
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

/**
 * @brief  响应DTO
 * @details
 * 响应数据传输对象，包含状态码和响应数据
 */
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

}  // namespace zipfiles::mp
#endif  // ! ZIPFILES_MQ_TYPE_H