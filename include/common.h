#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
#endif  // ! ZIPFILES_COMMON_H
