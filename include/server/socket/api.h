#ifndef ZIPFILES_MP_API_H
#define ZIPFILES_MP_API_H
#include "mp/Request.h"
namespace zipfiles::server::api {
void getCommitDetail(int client_fd, const Req& req);

void getCommitList(int client_fd, const Req& req);

void getFileList(int client_fd, const Req& req);

void getFileDetailList(int client_fd, const Req& req);

void postCommit(int client_fd, const Req& req);

void getFileDetail(int client_fd, const Req& req);

void mockNeedTime(int client_fd, const Req& req);

void mockManyNotifications(int client_fd, const Req& req);

void restore(int client_fd, const Req& req);

void logicDeleteCommit(int client_fd, const Req& req);

void physicalDeleteCommit(int client_fd, const Req& req);

void getCommitRecycleBin(int client_fd, const Req& req);

void recoverCommit(int client_fd, const Req& req);
}  // namespace zipfiles::server::api

#endif  // !ZIPFILES_MP_API_H