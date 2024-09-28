#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>

namespace zipfiles {

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
