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
      /**
       * @brief 请求调用getFileList
       *
       */
      if (request->is(mp::ApiType::GET_FILE_LIST)) {
        mp::ResponsePtr response = std::make_shared<mp::Response>();

        mp::GetFileListRequestPtr getFileListRequest =
          std::make_shared<mp::GetFileListRequest>();

        getFileListRequest->fromJson(request->getPayload());

        // 调用api.h中定义的fsapi函数
        mp::GetFileListResponsePtr getFileListResponse =
          api::getFileList(getFileListRequest);

        // todo: 错误处理
        // 设置返回的状态码
        response->setStatus(mp::StatusCode::OK);

        response->setPayload(getFileListResponse);

        if (!ServerSocket::send(response)) {
          std::cerr << "Failed to send response." << std::endl;
        }
      }

      /**
       * @brief 请求调用getFileDetail
       *
       */
      if (request->is(mp::ApiType::GET_FILE_DETAIL)) {
        mp::ResponsePtr response = std::make_shared<mp::Response>();

        mp::GetFileDetailRequestPtr getFileDetailRequest =
          std::make_shared<mp::GetFileDetailRequest>();

        // 从Json获得Request类实例
        getFileDetailRequest->fromJson(request->getPayload());

        // 调用api.h中定义的fsapi函数
        mp::GetFileDetailResponsePtr getFileDetailResponse =
          api::getFileDetail(getFileDetailRequest);

        // todo: 错误处理
        // 设置返回的状态码
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
