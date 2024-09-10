#ifndef ZIPFILES_MQ_TYPE_H
#define ZIPFILES_MQ_TYPE_H
#include <vector>
#include <string>
namespace zipfiles::mq {
extern const char* MQ_NAME;
enum class MessageType;
enum class ApiType;
enum class StatusCode;
struct Request;
struct Response;
const int MAX_MSG_SIZE = (1 << 10);
const int MAX_RESPONSE_CONTENT_SIZE =
  MAX_MSG_SIZE - sizeof(StatusCode) - sizeof(size_t);
}  // namespace zipfiles::mq
#include <cstddef>
namespace zipfiles::mq {

enum class ApiType { GET_FILE_LIST, GET_FILE };

enum class StatusCode { OK, ERROR };

class IContent {
 public:
  virtual void serialize(char* buffer, size_t& buffer_size) = 0;
  virtual void deserialize(const char* buffer, size_t& offset) = 0;
};

class IToString {
 public:
  virtual std::string to_string() = 0;
};

struct Request {
  ApiType api;
};

struct Response {
  StatusCode status;
  size_t contentSize;
  char content[MAX_RESPONSE_CONTENT_SIZE];
};
}  // namespace zipfiles::mq
#endif  // ! ZIPFILES_MQ_TYPE_H