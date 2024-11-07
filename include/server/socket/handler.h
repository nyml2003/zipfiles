#ifndef ZIPFILES_SERVER_HANDLER_H
#define ZIPFILES_SERVER_HANDLER_H

#include "mp/Request.h"
namespace zipfiles::server {

void doHandle(int client_fd, const Req& request);

}  // namespace zipfiles::server

#endif