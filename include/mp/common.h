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

/**
 * @brief 状态码
 * 1xx 请求顺利执行
 * 2xx 服务端异常
 * 3xx 客户端异常
 * 4xx Notification
 */
enum class Code {
  ILLEAGAL = 0,              // StatusCode为0的情况不合法
  OK = 100,                  // 正常返回
  ERROR = 200,               // 默认服务端异常
  CLIENT_ERROR = 300,        // 默认客户端异常
  NOTIFICATION = 400,        // 默认通知，应该是文本消息
  POSTCOMMIT_SUCCESS = 401,  // 提交Commit成功
  POSTCOMMIT_FAILED = 402,   // 提交Commit失败
  RESTORE_SUCCESS = 403,     // 恢复成功
  RESTORE_FAILED = 404,      // 恢复失败
};

enum class Api {
  ILLEAGAL = 0,    // ApiEnum为0的情况不合法
  NORESPONSE = 1,  // 请求途中发生错误，无法返回结果
  MOCK_MANY_NOTIFICATIONS = 98,
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
