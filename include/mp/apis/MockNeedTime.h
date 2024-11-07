#ifndef ZIPFILES_MP_APIS_MOCK_NEED_TIME_H
#define ZIPFILES_MP_APIS_MOCK_NEED_TIME_H

#include <memory>

namespace zipfiles {
struct Req;
using ReqPtr = std::shared_ptr<Req>;
struct Res;
using ResPtr = std::shared_ptr<Res>;

namespace request {
struct MockNeedTime {
  int id;
};
}  // namespace request

namespace response {

struct MockNeedTime {
  int id;
};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_MOCK_NEED_TIME_H