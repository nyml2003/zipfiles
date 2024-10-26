#ifndef ZIPFILES_SERVER_SELECTOR_H
#define ZIPFILES_SERVER_SELECTOR_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include "server/tools/threadpool.h"

namespace zipfiles::server {

/**
 * @brief 最大支持的并发线程数
 *
 */
constexpr int MAX_THREADS = 8;

class Selector {
 public:
  Selector() : tp(MAX_THREADS), connectionCount(0){};
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
  std::vector<uint8_t> read_buffer;
  std::vector<uint8_t> write_buffer;
  size_t read_buffer_ptr{};
  ThreadPool tp;
  std::atomic<int> connectionCount{};

  void parseJsonFromData(std::vector<uint8_t> data);
};

}  // namespace zipfiles::server

#endif