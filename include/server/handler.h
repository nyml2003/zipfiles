#ifndef ZIPFILES_SERVER_HANDLER_H
#define ZIPFILES_SERVER_HANDLER_H

#include "mp/apis/GetCommitDetail.h"
namespace zipfiles::server {

void doHandle(int client_fd, const ReqPtr& request);

void daemonize();

}  // namespace zipfiles::server

#endif