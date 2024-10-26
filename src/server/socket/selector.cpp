#include "server/selector.h"
#include <stdexcept>
#include <tuple>
#include "mp/mp.h"

namespace zipfiles::server {

void Selector::doSelect(int client_fd) {
  ssize_t bytesRead = read(client_fd, read_buffer.data(), mp::MAX_MESSAGE_SIZE);

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

  // todo: 放到类中
  static thread_local bool json_begin = false;
  static thread_local bool json_end = false;

  // read_buffer仍然有内容
  while (read_buffer_ptr <= read_buffer.size()) {
    write_buffer.push_back(read_buffer[read_buffer_ptr]);

    if (read_buffer[read_buffer_ptr] == '{') {
      json_begin = true;
    } else if (read_buffer[read_buffer_ptr] == '}') {
      json_end = true;
    }

    if (json_begin && json_end) {
      // todo: 解析
      parseJsonFromData(write_buffer);

      // todo: clear write_buffer
      // todo: 分发任务
      write_buffer.clear();
      json_begin = false;
      json_end = false;
      //   tp.enqueue();
    }
  }

  // 读完成
  // 可以清空read_buffer
  read_buffer.clear();
  read_buffer_ptr = 0;

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

}  // namespace zipfiles::server