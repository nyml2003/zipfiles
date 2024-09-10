#include "mp/mp.h"
#include "mp/GetFileList.h"
#include <iostream>

using namespace zipfiles::mp;

int main() {
  MessageQueue mq;

  // 接收请求
  Request request;
  if (mq.receiveRequest(request)) {
    if (request.is(ApiType::GET_FILE_LIST)) {
      std::vector<std::string> filenames = {
        "file1.txt", "file2.txt", "file3.txt"};

      Response response;
      response.setStatus(StatusCode::OK);
      GetFileListResponse getFileListResponse;
      getFileListResponse.setFilenames(filenames);
      response.setPayload(getFileListResponse);

      // 发送响应
      if (mq.sendResponse(response)) {
        std::cout << "Response sent successfully." << std::endl;
      } else {
        std::cerr << "Failed to send response." << std::endl;
      }
    } else {
      std::cerr << "Unknown API type." << std::endl;
    }
  } else {
    std::cerr << "Failed to receive request." << std::endl;
  }

  return 0;
}
