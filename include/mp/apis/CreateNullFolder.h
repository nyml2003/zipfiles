#ifndef ZIPFILES_MP_APIS_CREATE_NULL_FOLDER_H
#define ZIPFILES_MP_APIS_CREATE_NULL_FOLDER_H

#include <string>

namespace zipfiles {

namespace request {
struct CreateNullFolder {
  std::string path;
};
}  // namespace request

namespace response {

struct CreateNullFolder {};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_CREATE_NULL_FOLDER_H