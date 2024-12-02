#ifndef ZIPFILES_MP_APIS_PHYSICAL_DELETE_COMMIT_H
#define ZIPFILES_MP_APIS_PHYSICAL_DELETE_COMMIT_H

#include <string>
namespace zipfiles {

namespace request {
struct PhysicalDeleteCommit {
  std::string commitId;
};
}  // namespace request

namespace response {

struct PhysicalDeleteCommit {};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_PHYSICAL_DELETE_COMMIT_H