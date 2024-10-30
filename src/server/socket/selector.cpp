#include "server/selector.h"
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "mp/Request.h"
#include "mp/apis/GetCommitDetail.h"
#include "server/handler.h"
#include "server/socket/socket.h"

namespace zipfiles::server {

void Selector::doSelect(int client_fd) {
  try {
    Socket::receive(client_fd, read_buffer);
  } catch (std::exception& e) {
    subConnectionCount();

    throw std::runtime_error(
      "Failed to receive data from: " + std::to_string(client_fd) +
      ", becasue " + e.what()
    );
  }

  // read_buffer仍然有内容
  for (const uint8_t byte : read_buffer) {
    // log4cpp::Category::getRoot().infoStream()
    //   << "Read char: " << static_cast<char>(byte);

    switch (state) {
      case SelectorStatus::READ_DATA_SIZE:
        readDataSize(byte);
        break;
      case SelectorStatus::READ_DATA:
        if (readData(byte)) {
          parseJsonFromBuffer(client_fd);
          state = SelectorStatus::READ_DATA_SIZE;
        }
        break;
      default:
        throw std::runtime_error("Unknown state");
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

inline void Selector::readDataSize(uint8_t byte) {
  header_buffer.push_back(byte);

  if (header_buffer.size() == 4) {
    data_size = *reinterpret_cast<uint32_t*>(header_buffer.data());

    if (data_size == 0) {
      throw std::runtime_error("Illegal data_size, it is zero");
    }

    header_buffer.clear();
    state = SelectorStatus::READ_DATA;
  } else if (header_buffer.size() > 4) {
    throw std::runtime_error("Illegal header buffer size");
  }
}

inline bool Selector::readData(uint8_t byte) {
  write_buffer.push_back(byte);

  return data_size == write_buffer.size();
}

void Selector::parseJsonFromBuffer(int client_fd) {
  Json::Reader reader;
  Json::Value jsonData;
  std::string jsonString(write_buffer.begin(), write_buffer.end());

  if (reader.parse(jsonString, jsonData)) {
    ReqPtr request = Req::fromJson(jsonData);

    tp.enqueue([client_fd, request]() { doHandle(client_fd, request); });
  } else {
    write_buffer.clear();
    throw std::runtime_error(&"Illegal json format when reading: "[client_fd]);
  }

  write_buffer.clear();
}

}  // namespace zipfiles::server