#ifndef ZIPFILES_MP_APIS_MOCK_MANY_NOTIFICATIONS_H
#define ZIPFILES_MP_APIS_MOCK_MANY_NOTIFICATIONS_H

#include <string>
namespace zipfiles {

namespace request {
struct MockManyNotifications {};
}  // namespace request

namespace response {

struct MockManyNotifications {
  std::string message;
};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_MOCK_MANY_NOTIFICATIONS_H