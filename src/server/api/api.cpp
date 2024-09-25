#include "server/api/api.h"
#include "server/tools/fsapi.h"
#include <vector>

namespace zipfiles::server::api {
mp::ServerSocket& serverSocket = mp::ServerSocket::getInstance();
mp::GetFileListResponsePtr getFileList(
  const mp::GetFileListRequestPtr& getFileListRequest
) {
  std::vector<zipfiles::File> files =
    server::getFilesList(getFileListRequest->getPath());
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