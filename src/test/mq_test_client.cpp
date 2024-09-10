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
    int input;
    std::cout << "输入1以发送请求，输入其他值以退出: ";
    std::cin >> input;

    if (input != 1) {
      break;
    }

    // 创建请求
    Request req;
    req.api = ApiType::GET_FILE_LIST;

    // 发送请求
    if (!mqWrapper.sendRequest(req)) {
      std::cerr << "Failed to send request" << std::endl;
      continue;
    }

    // 接收响应
    Response res;
    if (!mqWrapper.receiveResponse(res)) {
      std::cerr << "Failed to receive response" << std::endl;
      continue;
    }

    // 处理响应
    if (res.status == StatusCode::OK) {
      size_t offset = 0;
      GetFileList::GetFileList fileList;
      fileList.deserialize(res.content, offset);
      std::cout << "File list: " << fileList.to_string() << std::endl;
    } else {
      std::cerr << "Error in response" << std::endl;
    }
  }

  return 0;
}