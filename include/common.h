#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H

namespace zipfiles {
template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;
}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
