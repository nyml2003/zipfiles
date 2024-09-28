#ifndef ZIPFILES_MP_H
#define ZIPFILES_MP_H
#include <json/json.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstddef>
namespace zipfiles::mp {
/**
 * @brief 最大消息长度= 2^16B = 64KB
 */
constexpr size_t MAX_MESSAGE_SIZE = (1 << 16);
/**
 * @brief 服务器端口
 */
constexpr size_t PORT = 8080;

}  // namespace zipfiles::mp
#endif  // ! ZIPFILES_MQ_TYPE_H