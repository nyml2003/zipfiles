#include "mq/mq.h"
#include "mq/mqwrapper.h"
#include "mq/getfilelist.h"
#include <iostream>
#include <vector>

int main() {
  using namespace zipfiles::mq;

  // 创建消息队列包装器
  MQWrapper mqWrapper;

  while (true) {
    // 接收请求
    Request req;
    if (!mqWrapper.receiveRequest(req)) {
      std::cerr << "Failed to receive request" << std::endl;
      continue;
    }

    // 处理请求并创建响应
    Response res;
    res.status = StatusCode::OK;

    if (req.api == ApiType::GET_FILE_LIST) {
      std::vector<std::string> filenames = {
        "/app/include/mq/GetFileList.h", "build/cmake_install.cmake123",
        "/app/gui/src/apis/Sum.ts"};
      GetFileList::GetFileList fileList(filenames);
      size_t offset = 0;
      fileList.serialize(res.content, offset);
      res.contentSize = offset;
    } else {
      res.status = StatusCode::ERROR;
    }

    // 发送响应
    if (!mqWrapper.sendResponse(res)) {
      std::cerr << "Failed to send response" << std::endl;
      continue;
    }
  }

  return 0;
}