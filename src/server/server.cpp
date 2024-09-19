#include "server/api/api.h"
#include "mp/mp.h"
#include <iostream>
namespace zipfiles::server {
void run() {
  mp::ServerSocket& serverSocket = mp::ServerSocket::getInstance();
  while (true) {
    serverSocket.acceptConnection();
    mp::RequestPtr request = std::make_shared<mp::Request>();
    try {
      while (serverSocket.receive(request)) {
        if (request->is(mp::ApiType::GET_FILE_LIST)) {
          mp::ResponsePtr response = std::make_shared<mp::Response>();
          mp::GetFileListRequestPtr getFileListRequest =
            std::make_shared<mp::GetFileListRequest>();
          getFileListRequest->fromJson(request->getPayload());
          mp::GetFileListResponsePtr getFileListResponse =
            api::getFileList(getFileListRequest);
          response->setStatus(mp::StatusCode::OK);
          response->setPayload(getFileListResponse);
          if (!serverSocket.send(response)) {
            std::cerr << "Failed to send response." << std::endl;
          }
        }
      }
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
    std::cout << "Waiting for new client connection..." << std::endl;
  }
}
}  // namespace zipfiles::server

int main() {
  zipfiles::server::run();
  return 0;
}
