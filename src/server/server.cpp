#include "mp/mp.h"
#include "mp/GetFileList.h"
#include "server/list.h"
#include <iostream>

using namespace zipfiles::mp;

int main() {
  MessageQueue mq;
  while (true) {
    try {
      Request request;
      if (!mq.receiveRequest(request)) {
        throw std::runtime_error("Failed to receive request.");
        continue;
      }
      if (!request.is(ApiType::GET_FILE_LIST)) {
        throw std::runtime_error("Unknown API type.");
        continue;
      }
      GetFileListRequest getFileListRequest;
        getFileListRequest.fromJson(request.getPayload());

      std::vector<File> files = zipfiles::server::list(getFileListRequest.getPath());
      Response response;
      response.setStatus(StatusCode::OK);
      GetFileListResponse getFileListResponse;
      getFileListResponse.setFiles(files);
      response.setPayload(getFileListResponse);
      std::cout << "Sending response: " << response.toJson() << std::endl;
      if (!mq.sendResponse(response)) {
        throw std::runtime_error("Failed to send response.");
      }
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }

  return 0;
}
