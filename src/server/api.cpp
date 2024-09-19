#include "server/api/api.h"
#include "server/tools/fsapi.h"
#include <vector>

namespace zipfiles::server::api {
mp::ServerSocket& serverSocket = mp::ServerSocket::getInstance();
mp::GetFileListResponsePtr getFileList(
  const mp::GetFileListRequestPtr& getFileListRequest
) {
  std::vector<zipfiles::File> files =
    zipfiles::server::list(getFileListRequest->getPath());
  mp::GetFileListResponsePtr getFileListResponse =
    std::make_shared<mp::GetFileListResponse>();
  getFileListResponse->setFiles(files);
  return getFileListResponse;
}
}  // namespace zipfiles::server::api