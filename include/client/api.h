#ifndef ZIPFILE_CLIENT_API_H
#define ZIPFILE_CLIENT_API_H
#include <future>
#include "mp/GetFileList.h"
#include "mp/GetFileDetail.h"
namespace zipfiles::client::api {
/**
 * @brief 获取文件列表
 * @details 从socket中获取文件列表，并做一些异常处理
 * @param request
 * @return mp::GetFileListResponsePtr
 */
std::future<mp::GetFileDetailResponsePtr> getFileDetailAsync(
  const mp::GetFileDetailRequestPtr& getFileDetailRequest
);
/**
 * @brief 获取文件详情
 * @details 从socket中获取文件详情，并做一些异常处理
 * @param request
 * @return mp::GetFileDetailResponsePtr
 */
std::future<mp::GetFileListResponsePtr> getFileListAsync(
  const mp::GetFileListRequestPtr& getFileListRequest
);
}  // namespace zipfiles::client::api

#endif  // !ZIPFILE_CLIENT_API_H