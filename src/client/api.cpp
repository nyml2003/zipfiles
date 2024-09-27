#include "client/api.h"
#include "client/socket.h"
#include <iostream>
#include <future>

namespace zipfiles::client::api {

std::future<mp::GetFileListResponsePtr> getFileListAsync(
  const mp::GetFileListRequestPtr& getFileListRequest
) {
  return std::async(std::launch::async, [getFileListRequest]() {
    mp::RequestPtr request = std::make_shared<mp::Request>();
    request->setApi(mp::ApiType::GET_FILE_LIST);
    getFileListRequest->setPath(getFileListRequest->getPath());
    request->setPayload(getFileListRequest);
    if (!socket::ClientSocket::send(request)) {
      throw std::runtime_error("Failed to send request.");
    }
    std::cout << "Sent request and waiting for response." << std::endl;
    mp::ResponsePtr response = std::make_shared<mp::Response>();
    if (!socket::ClientSocket::receive(response)) {
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
  });
}

std::future<mp::GetFileDetailResponsePtr> getFileDetailAsync(
  const mp::GetFileDetailRequestPtr& getFileDetailRequest
) {
  return std::async(std::launch::async, [getFileDetailRequest]() {
    mp::RequestPtr request = std::make_shared<mp::Request>();
    request->setApi(mp::ApiType::GET_FILE_DETAIL);
    getFileDetailRequest->setPath(getFileDetailRequest->getPath());
    request->setPayload(getFileDetailRequest);
    if (!socket::ClientSocket::send(request)) {
      throw std::runtime_error("Failed to send request.");
    }
    mp::ResponsePtr response = std::make_shared<mp::Response>();
    if (!socket::ClientSocket::receive(response)) {
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
  });
}

}  // namespace zipfiles::client::api