#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H
#include <ctime>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>

namespace zipfiles {
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
namespace fs = std::filesystem;

struct File {
  std::string name;
  fs::file_type type;
};

struct FileDetail {
  fs::file_type type;
  std::string createTime;
  std::string updateTime;
  __off_t size;
  std::string owner;
  std::string group;
  mode_t mode;
  std::string path;
  std::string name;
};

}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
