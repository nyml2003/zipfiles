#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H
#include <string>
namespace zipfiles {
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

enum class FileType { FILE, DIRECTORY };

struct File {
  std::string name;
  FileType type;
};

}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
