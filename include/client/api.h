#ifndef ZIPFILE_CLIENT_API_H
#define ZIPFILE_CLIENT_API_H
#include "mp/GetFileList.h"
namespace zipfiles::client::api {
/**
 * @brief 获取文件列表
 * @details 从socket中获取文件列表，并做一些异常处理
 * @param request
 * @return mp::GetFileListResponsePtr
 */
mp::GetFileListResponsePtr getFileList(const mp::GetFileListRequestPtr& request
);
}  // namespace zipfiles::client::api

#endif  // !ZIPFILE_CLIENT_API_H