#include "server/handler.h"
#include <iostream>
#include "server/api/api.h"
#include "mp/mp.h"
#include "server/socket/socket.h"

namespace zipfiles::server {

void doHandle() {
  mp::RequestPtr request = std::make_shared<mp::Request>();

  try {
    // 主eventloop
    while (ServerSocket::receive(request)) {
      if (request->is(mp::ApiType::GET_FILE_LIST)) {
        mp::ResponsePtr response = std::make_shared<mp::Response>();

        mp::GetFileListRequestPtr getFileListRequest =
          std::make_shared<mp::GetFileListRequest>();

        getFileListRequest->fromJson(request->getPayload());

        mp::GetFileListResponsePtr getFileListResponse =
          api::getFileList(getFileListRequest);

        response->setStatus(mp::StatusCode::OK);

        response->setPayload(getFileListResponse);

        if (!ServerSocket::send(response)) {
          std::cerr << "Failed to send response." << std::endl;
        }
      }

      if (request->is(mp::ApiType::GET_FILE_DETAIL)) {
        mp::ResponsePtr response = std::make_shared<mp::Response>();

        mp::GetFileDetailRequestPtr getFileDetailRequest =
          std::make_shared<mp::GetFileDetailRequest>();

        getFileDetailRequest->fromJson(request->getPayload());

        mp::GetFileDetailResponsePtr getFileDetailResponse =
          api::getFileDetail(getFileDetailRequest);

        response->setStatus(mp::StatusCode::OK);

        response->setPayload(getFileDetailResponse);

        if (!ServerSocket::send(response)) {
          std::cerr << "Failed to send response." << std::endl;
        }
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}
}  // namespace zipfiles::server
