#include "client/api.h"
#include <iostream>

namespace zipfiles::client::api {
mp::ClientSocket& clientSocket = mp::ClientSocket::getInstance();

mp::GetFileListResponsePtr getFileList(
  const mp::GetFileListRequestPtr& getFileListRequest
) {
  mp::RequestPtr request = std::make_shared<mp::Request>();
  request->setApi(mp::ApiType::GET_FILE_LIST);
  getFileListRequest->setPath(getFileListRequest->getPath());
  request->setPayload(getFileListRequest);
  if (!clientSocket.send(request)) {
    throw std::runtime_error("Failed to send request.");
  }
  mp::ResponsePtr response = std::make_shared<mp::Response>();
  if (!clientSocket.receive(response)) {
    throw std::runtime_error("Failed to receive response.");
  }
  if (!response->is(mp::StatusCode::OK)) {
    throw std::runtime_error("Error in response.");
  }
  mp::GetFileListResponsePtr getFileListResponse =
    std::make_shared<mp::GetFileListResponse>();
  getFileListResponse->fromJson(response->getPayload());
  std::cout << "Received response: " << getFileListResponse->toJson()
            << std::endl;
  return getFileListResponse;
}

}  // namespace zipfiles::client::api