#include "client/api.h"
#include <iostream>

namespace zipfiles::client::api {

mp::GetFileListResponsePtr getFileList(
  const mp::GetFileListRequestPtr& getFileListRequest
) {
  mp::RequestPtr request = std::make_shared<mp::Request>();
  request->setApi(mp::ApiType::GET_FILE_LIST);
  getFileListRequest->setPath(getFileListRequest->getPath());
  request->setPayload(getFileListRequest);
  if (!mp::ClientSocket::send(request)) {
    throw std::runtime_error("Failed to send request.");
  }
  mp::ResponsePtr response = std::make_shared<mp::Response>();
  if (!mp::ClientSocket::receive(response)) {
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

mp::GetFileDetailResponsePtr getFileDetail(
  const mp::GetFileDetailRequestPtr& getFileDetailRequest
) {
  mp::RequestPtr request = std::make_shared<mp::Request>();
  request->setApi(mp::ApiType::GET_FILE_DETAIL);
  getFileDetailRequest->setPath(getFileDetailRequest->getPath());
  request->setPayload(getFileDetailRequest);
  if (!mp::ClientSocket::send(request)) {
    throw std::runtime_error("Failed to send request.");
  }
  mp::ResponsePtr response = std::make_shared<mp::Response>();
  if (!mp::ClientSocket::receive(response)) {
    throw std::runtime_error("Failed to receive response.");
  }
  if (!response->is(mp::StatusCode::OK)) {
    throw std::runtime_error("Error in response.");
  }
  mp::GetFileDetailResponsePtr getFileDetailResponse =
    std::make_shared<mp::GetFileDetailResponse>();
  getFileDetailResponse->fromJson(response->getPayload());
  std::cout << "Received response: " << getFileDetailResponse->toJson()
            << std::endl;
  return getFileDetailResponse;
}

}  // namespace zipfiles::client::api