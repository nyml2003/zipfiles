#ifndef ZIPFILE_SERVER_API_H
#define ZIPFILE_SERVER_API_H
#include "mp/GetFileList.h"
#include "mp/GetFileDetail.h"
// 调用server能力, 解包请求, 打包响应
namespace zipfiles::server::api {
/**
 * @brief 获取文件列表
 * @details 调用fsapi的方法获取文件列表，并构造响应
 * @param getFileListRequest 请求
 * @return GetFileListResponsePtr 响应
 */
mp::GetFileListResponsePtr getFileList(
  const mp::GetFileListRequestPtr& getFileListRequest
);
/**
 * @brief 获取文件详情
 * @details 调用fsapi的方法获取文件详情，并构造响应
 * @param getFileDetailRequest 请求
 * @return GetFileDetailResponsePtr 响应
 */
mp::GetFileDetailResponsePtr getFileDetail(
  const mp::GetFileDetailRequestPtr& getFileDetailRequest
);
}  // namespace zipfiles::server::api

#endif  // !ZIPFILE_CLIENT_API_H