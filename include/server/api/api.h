#ifndef ZIPFILE_SERVER_API_H
#define ZIPFILE_SERVER_API_H
#include "mp/GetFileList.h"
#include "mp/GetFileDetail.h"
#include "mp/mp.h"
namespace zipfiles::server::api {
extern mp::ServerSocket& serverSocket;
mp::GetFileListResponsePtr getFileList(
  const mp::GetFileListRequestPtr& getFileListRequest
);
mp::GetFileDetailResponsePtr getFileDetail(
  const mp::GetFileDetailRequestPtr& getFileDetailRequest
);
}  // namespace zipfiles::server::api

#endif  // !ZIPFILE_CLIENT_API_H