#ifndef ZIPFILES_SERVER_ACCEPTOR_H
#define ZIPFILES_SERVER_ACCEPTOR_H

namespace zipfiles::server {
/**
 * @brief 最大支持的epoll_event数
 *
 */
constexpr int MAX_EPOLL_EVENTS = 1024;

void doAccept();

}  // namespace zipfiles::server

#endif