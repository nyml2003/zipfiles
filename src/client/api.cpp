#include "client/api.h"
#include <iostream>

namespace zipfiles::client::api {
using namespace zipfiles::mp;
ClientSocket clientSocket;
GetFileListResponse getFileList(GetFileListRequest getFileListRequest) {
  Request request;
  request.setApi(ApiType::GET_FILE_LIST);
getFileListRequest.setPath(getFileListRequest.getPath());
request.setPayload(getFileListRequest);
if (!clientSocket.send(request)) {
  throw std::runtime_error("Failed to send request.");
}
  Response response;
  if (!clientSocket.receive(response)) {
    throw std::runtime_error("Failed to receive response.");
  }
  if (!response.is(StatusCode::OK)) {
    throw std::runtime_error("Error in response.");
  }
  GetFileListResponse getFileListResponse;
  getFileListResponse.fromJson(response.getPayload());
  for (const auto& file : getFileListResponse.getFiles()) {
    std::cout << file.name << " " << (file.type == FileType::FILE ? "file" : "directory") << std::endl;
  }
  return getFileListResponse;
}

}  // namespace zipfiles::client::api