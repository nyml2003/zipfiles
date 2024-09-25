#include <future>
#include <iostream>
#include <vector>
#include <sys/select.h>
#include <unistd.h>
#include "server/api/api.h"
#include "mp/mp.h"

namespace zipfiles::server {

void handleClient(mp::ServerSocket& serverSocket) {
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
}

void run() {
  mp::ServerSocket& serverSocket = mp::ServerSocket::getInstance();
  int serverFd = serverSocket.getSocketFd();

  fd_set readfds;
  std::vector<std::future<void>> futures;  // 用于存储 future 对象

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(serverFd, &readfds);

    int activity = select(serverFd + 1, &readfds, nullptr, nullptr, nullptr);
    if (activity < 0) {
      std::cerr << "Select error" << std::endl;
      break;
    }

    if (FD_ISSET(serverFd, &readfds)) {
      serverSocket.acceptConnection();
      futures.emplace_back(
        std::async(std::launch::async, handleClient, std::ref(serverSocket))
      );
      std::cout << "Waiting for new client connection..." << std::endl;
    }
  }
}

}  // namespace zipfiles::server

int main() {
  zipfiles::server::run();
  return 0;
}