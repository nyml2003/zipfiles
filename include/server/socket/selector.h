#ifndef ZIPFILES_SERVER_SELECTOR_H
#define ZIPFILES_SERVER_SELECTOR_H

#include <cstdint>
#include <vector>
#include "server/tools/threadpool.h"

namespace zipfiles::server {

/**
 * @brief 最大支持的并发线程数
 *
 */
constexpr int MAX_THREADS = 8;

enum class SelectorStatus { READ_DATA_SIZE, READ_DATA };

class Selector {
 public:
  Selector() : tp(MAX_THREADS), connectionCount(0) {};
  ~Selector() = default;
  void doSelect(int client_fd);
  int getConnectionCount();
  void addConnectionCount();
  void subConnectionCount();

  Selector(const Selector&) = delete;
  Selector& operator=(const Selector&) = delete;
  Selector(Selector&&) = delete;
  Selector& operator=(Selector&&) = delete;

 private:
  std::vector<uint8_t> header_buffer;
  std::vector<uint8_t> read_buffer;
  std::vector<uint8_t> write_buffer;
  uint32_t data_size{};
  SelectorStatus state{};
  ThreadPool tp;
  std::atomic<int> connectionCount{};

  void readDataSize(uint8_t byte);
  bool readData(uint8_t byte);
  void parseJsonFromBuffer(int client_fd);
};

}  // namespace zipfiles::server

#endif