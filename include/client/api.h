#ifndef ZIPFILE_CLIENT_API_H
#define ZIPFILE_CLIENT_API_H
#include "mp/GetFileList.h"
#include "mp/mp.h"
namespace zipfiles::client::api {
extern mp::ClientSocket& clientSocket;
mp::GetFileListResponsePtr getFileList(const mp::GetFileListRequestPtr& request
);
}  // namespace zipfiles::client::api

#endif  // !ZIPFILE_CLIENT_API_H