
#ifndef ZIPFILES_MP_APIS_LOGIC_DELETE_COMMIT_H
#define ZIPFILES_MP_APIS_LOGIC_DELETE_COMMIT_H
#include <string>

namespace zipfiles {

namespace request {
struct LogicDeleteCommit {
  std::string commitId;
};
}  // namespace request

namespace response {

struct LogicDeleteCommit {};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_LOGIC_DELETE_COMMIT_H