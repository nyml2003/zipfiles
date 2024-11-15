#ifndef ZIPFILES_MP_APIS_RESTORE_H
#define ZIPFILES_MP_APIS_RESTORE_H

#include <optional>
#include <string>

namespace zipfiles {

namespace request {
struct Restore {
  std::optional<std::string> key;
  std::string commitId;  // 本次恢复对应的commitId
  std::string path;
  std::string messageId;  // 耗时请求对应的消息id
};
}  // namespace request

namespace response {

struct Restore {};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_RESTORE_H