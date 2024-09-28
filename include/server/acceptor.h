#ifndef ZIPFILES_SERVER_ACCEPTOR_H
#define ZIPFILES_SERVER_ACCEPTOR_H

/**
 * @brief 最大支持的并发线程数
 *
 */
#define MAX_THREADS 8

namespace zipfiles::server {

void doAccept();

}

#endif