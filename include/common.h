#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace zipfiles {
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
