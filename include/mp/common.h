#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H

#include <cstddef>
#include <stdexcept>

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
 * 1xx 请求顺利执行 Api一定是Remote Function
 * 2xx 服务端异常 Api一定是NORESPONSE
 * 3xx 客户端异常 Api一定是NORESPONSE
 * 4xx 通知 需要前端根据Code区分
 *
 * 40x 单行文本 x为0,1,2,3 分别表示success, warning, error, info,
 * 单行是指只有title
 *
 * 41x 双行文本 x为0,1,2,3 分别表示success, warning, error,info,
 * 双行是指有title和description
 *
 * 42x 创建备份
 * x为0表示备份成功,
 * 1表示备份失败，字段有messageId和description
 *
 * 43x 恢复备份
 * x为0表示恢复成功,
 * 1表示恢复失败，字段有messageId和description
 */
enum class Code {
  ILLEAGAL = 0,           // StatusCode为0的情况不合法
  OK = 100,               // 正常返回
  SERVER_ERROR = 200,     // 默认服务端异常
  CLIENT_ERROR = 300,     // 默认客户端异常
  SINGLE_SUCCESS = 400,   // 单行文本 success
  SINGLE_WARNING = 401,   // 单行文本 warning
  SINGLE_ERROR = 402,     // 单行文本 error
  SINGLE_INFO = 403,      // 单行文本 info
  DOUBLE_SUCCESS = 410,   // 双行文本 success
  DOUBLE_WARNING = 411,   // 双行文本 warning
  DOUBLE_ERROR = 412,     // 双行文本 error
  DOUBLE_INFO = 413,      // 双行文本 info
  BACKUP_SUCCESS = 420,   // 备份成功
  BACKUP_FAILED = 421,    // 备份失败
  RESTORE_SUCCESS = 430,  // 恢复成功
  RESTORE_FAILED = 431,   // 恢复失败
};
/**
 * @brief: 定义所有的接口枚举
 * 数字为1xx的为Remote Function
 * 数字为9x的为Mock Function
 */
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
  LOGIC_DELETE_COMMIT = 107,
  PHYSICAL_DELETE_COMMIT = 108,
  GET_COMMIT_RECYCLE_BIN = 109,
  RECOVER_COMMIT = 110,
};

class UnknownApiException : public std::runtime_error {
 public:
  explicit UnknownApiException(const std::string& message)
    : std::runtime_error(message) {}
};

}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
