#ifndef ZIPFILES_SERVER_HANDLER_H
#define ZIPFILES_SERVER_HANDLER_H

namespace zipfiles::server {

void doHandle(int client_fd);
void daemonize();
}  // namespace zipfiles::server

#endif