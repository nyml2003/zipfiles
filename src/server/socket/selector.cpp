#include "server/selector.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/apis/GetCommitDetail.h"
#include "mp/mp.h"
#include "server/handler.h"

namespace zipfiles::server {

void Selector::doSelect(int client_fd) {
  read_buffer.resize(mp::MAX_MESSAGE_SIZE);  // 预留空间

  ssize_t bytesRead = read(client_fd, read_buffer.data(), mp::MAX_MESSAGE_SIZE);

  if (bytesRead > 0) {
    read_buffer.resize(bytesRead);  // 调整缓冲区大小以适应实际读取的数据
  }

  // 连接是否关闭
  if (bytesRead == 0) {
    // 断开连接
    close(client_fd);

    // 减少连接数
    subConnectionCount();

    throw std::runtime_error("Client disconnected");
  }

  // socket是否产生了错误
  if (bytesRead < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      std::stringstream ss;
      ss << client_fd;
      ss << " has no more data, now disconnect";
      // 没有更多数据可读
      throw std::runtime_error(ss.str());
    }

    close(client_fd);

    subConnectionCount();

    throw std::runtime_error("Failed to receive request, now disconnect");
  }

  // read_buffer仍然有内容
  for (const uint8_t byte : read_buffer) {
    write_buffer.push_back(byte);

    if (isValidJson(byte)) {
      Json::Reader reader;
      Json::Value jsonData;
      std::string jsonString(write_buffer.begin(), write_buffer.end());

      if (reader.parse(jsonString, jsonData)) {
        ReqPtr request = Req::fromJson(jsonData);

        tp.enqueue([client_fd, request]() { doHandle(client_fd, request); });
      } else {
        resetWriteBuffer(write_buffer);
        throw std::runtime_error(
          &"Illegal json format when reading: "[client_fd]
        );
      }

      resetWriteBuffer(write_buffer);
      //   tp.enqueue();
    }
  }

  // 读完成
  // 可以清空read_buffer
  read_buffer.clear();

  // ? 可以不swap，因为swap后还需要重新读一遍write_buffer的剩余内容
  //   std::swap(read_buffer, write_buffer);
}

int Selector::getConnectionCount() {
  return connectionCount.load();
}

void Selector::addConnectionCount() {
  connectionCount++;
}

void Selector::subConnectionCount() {
  connectionCount--;
}

void Selector::resetWriteBuffer(std::vector<uint8_t>& write_buffer) {
  write_buffer.clear();
  parsing = false;
  braceCount = 0;
  bracketCount = 0;
}

bool Selector::isValidJson(const uint8_t& byte) {
  char ch = static_cast<char>(byte);

  if (ch == '{') {
    braceCount++;
    parsing = true;
  } else if (ch == '}') {
    braceCount--;
  } else if (ch == '[') {
    bracketCount++;
    parsing = true;
  } else if (ch == ']') {
    bracketCount--;
  }

  return parsing && braceCount == 0 && bracketCount == 0;
}

}  // namespace zipfiles::server