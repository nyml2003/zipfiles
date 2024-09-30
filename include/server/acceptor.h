#ifndef ZIPFILES_SERVER_ACCEPTOR_H
#define ZIPFILES_SERVER_ACCEPTOR_H

/**
 * @brief 最大支持的并发线程数
 *
 */
#define MAX_THREADS 8

/**
 * @brief 最大支持的epoll_event数
 *
 */
#define MAX_EPOLL_EVENTS 16

namespace zipfiles::server {

void doAccept();

}

#endif