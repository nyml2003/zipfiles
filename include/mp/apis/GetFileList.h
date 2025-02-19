#ifndef ZIPFILES_MP_APIS_GET_FILE_LIST_H
#define ZIPFILES_MP_APIS_GET_FILE_LIST_H

#include <sys/types.h>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
namespace zipfiles {

namespace request {

struct GetFileList {
  std::string path;
};
}  // namespace request

namespace response {

namespace getFileList {
struct File {
  // 文件类型，定义见filesystem
  fs::file_type type{};
  // 文件名
  std::string name;
};
}  // namespace getFileList

struct GetFileList {
  std::vector<getFileList::File> files;
};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_GET_FILE_LIST_H