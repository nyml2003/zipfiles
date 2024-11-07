#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H

#include <cstddef>

namespace zipfiles {

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

/**
 * @brief 最大消息长度= 2^16B = 64KB
 */
constexpr size_t MAX_MESSAGE_SIZE = (1 << 16);
/**
 * @brief 服务器端口
 */
constexpr size_t PORT = 8080;

enum class Code {
  ILLEAGAL = 0,      // StatusCode为0的情况不合法
  OK = 1,            // 正常返回
  ERROR = 2,         // 出现异常(默认，可自定义其他StatusCode)
  NOTIFICATION = 3,  // 后端主动向前端返回消息
  CLIENT_ERROR = 4,  // 客户端异常
};

enum class Api {
  ILLEAGAL = 0,  // ApiEnum为0的情况不合法
  MOCK_NEED_TIME = 99,
  GET_FILE_LIST = 100,
  GET_FILE_DETAIL_LIST = 101,
  POST_COMMIT = 102,
  GET_COMMIT_DETAIL = 103,
  GET_COMMIT_LIST = 104,
  GET_FILE_DETAIL = 105,
  RESTORE = 106,
};

}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
