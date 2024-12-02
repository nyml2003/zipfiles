#ifndef ZIPFILES_MP_APIS_RECOVER_COMMIT_H
#define ZIPFILES_MP_APIS_RECOVER_COMMIT_H

#include <string>
namespace zipfiles {

namespace request {
struct RecoverCommit {
  std::string commitId;
};
}  // namespace request

namespace response {

struct RecoverCommit {};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_RECOVER_COMMIT_H