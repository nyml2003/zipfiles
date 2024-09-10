#ifndef ZIPFILE_CLIENT_API_H
#define ZIPFILE_CLIENT_API_H
#include "mp/GetFileList.h"
namespace zipfiles::client::api {
using namespace zipfiles::mp;
extern MessageQueue mq;
GetFileListResponse getFileList(GetFileListRequest getFileListRequest);

}  // namespace zipfiles::client::api

#endif  // !ZIPFILE_CLIENT_API_H