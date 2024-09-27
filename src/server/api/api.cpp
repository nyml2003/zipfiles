#include "server/api/api.h"
#include "common.h"
#include "server/tools/fsapi.h"
#include <filesystem>
#include <vector>

namespace zipfiles::server::api {

mp::GetFileListResponsePtr getFileList(
  const mp::GetFileListRequestPtr& getFileListRequest
) {
  // 初始化filter
  // todo: 依据request传来的过滤需求初始化filter
  MetaDataFilter filter;

  // 调用fsapi中的getFilesList
  std::vector<zipfiles::File> files =
    server::getFilesList(getFileListRequest->getPath(), false, filter);

  // 依据返回的文件列表初始化response
  mp::GetFileListResponsePtr getFileListResponse =
    std::make_shared<mp::GetFileListResponse>();

  getFileListResponse->setFiles(files);

  return getFileListResponse;
}

mp::GetFileDetailResponsePtr getFileDetail(
  const mp::GetFileDetailRequestPtr& getFileDetailRequest
) {
  FileDetail metadata = server::getFileDetail(getFileDetailRequest->getPath());

  mp::GetFileDetailResponsePtr getFileDetailResponse =
    std::make_shared<mp::GetFileDetailResponse>();

  getFileDetailResponse->setMetadata(metadata);

  return getFileDetailResponse;
}

}  // namespace zipfiles::server::api