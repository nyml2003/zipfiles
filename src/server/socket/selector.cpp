#include "server/socket/selector.h"

#include <cstdint>
#include <exception>
#include <log4cpp/Category.hh>
#include <stdexcept>
#include <string>
#include <vector>
#include "mp/Request.h"
#include "mp/apis/GetCommitDetail.h"
#include "server/socket/handler.h"
#include "server/socket/socket.h"

namespace zipfiles::server {
/**
 * @brief selector实现，其每次从socket读取socket buffer中的所有数据，并进行解析
 *
 * @param client_fd client
 *
 */
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

/**
 * @brief 获取当前的连接数
 *
 * @return int 当前的连接数
 *
 */
int Selector::getConnectionCount() {
  return connectionCount.load();
}

/**
 * @brief 当前连接数加一
 *
 */
void Selector::addConnectionCount() {
  connectionCount++;
}

/**
 * @brief 当前连接数减一
 *
 */
void Selector::subConnectionCount() {
  connectionCount--;
}

/**
 * @brief 试图从读取的数据中解析出数据大小
 *
 * @param byte 读取的数据的一个byte
 *
 */
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

/**
 * @brief 试图从读取的数据中读取出数据内容
 *
 * @param byte 读取的数据的一个byte
 *
 */
inline bool Selector::readData(uint8_t byte) {
  write_buffer.push_back(byte);

  return data_size == write_buffer.size();
}

/**
 * @brief 从读取的数据内容中解析出buffer，并分发任务到线程池
 *
 * @param client_fd selector正在处理的client
 *
 */
void Selector::parseJsonFromBuffer(int client_fd) {
  Json::Reader reader;
  Json::Value jsonData;
  std::string jsonString(write_buffer.begin(), write_buffer.end());

  if (reader.parse(jsonString, jsonData)) {
    log4cpp::Category::getRoot().infoStream() << "Parsed json: " << jsonData;
    Req req = Req::fromJson(jsonData);

    tp.enqueue([client_fd, req]() { doHandle(client_fd, req); });
  } else {
    write_buffer.clear();
    throw std::runtime_error(&"Illegal json format when reading: "[client_fd]);
  }

  write_buffer.clear();
}

}  // namespace zipfiles::server