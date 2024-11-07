#include "server/selector.h"
#include <fcntl.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace zipfiles::server {

class SelectorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(server_fd, -1);

    // 设置SO_REUSEADDR选项
    int optval = 1;
    ASSERT_EQ(
      setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)),
      0
    );

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    ASSERT_EQ(bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)), 0);
    ASSERT_EQ(listen(server_fd, 1), 0);

    // 设置非阻塞模式
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
  }

  void TearDown() override { close(server_fd); }

  int server_fd;
  Selector selector;
};

// 创建客户端并连接到服务器
int createClient() {
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = INADDR_ANY;

  int result =
    connect(client_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));

  return client_fd;
}

// 测试成功读取数据的情况
TEST_F(SelectorTest, SuccessfulRead) {
  int client_fd = createClient();

  int conn_fd = 0;
  while ((conn_fd = accept(server_fd, nullptr, nullptr)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      usleep(1000);  // 等待一小段时间后重试
    } else {
      std::cerr << "接受连接时出错: " << strerror(errno) << std::endl;
      break;
    }
  }
  ASSERT_NE(conn_fd, -1);

  std::string test_data = R"({"key": "value"})";
  send(client_fd, test_data.c_str(), test_data.size(), 0);
  shutdown(client_fd, SHUT_WR);

  ASSERT_NO_THROW(selector.doSelect(conn_fd));
  close(client_fd);
  close(conn_fd);
}

// 测试客户端断开连接的情况
TEST_F(SelectorTest, ClientDisconnect) {
  int client_fd = createClient();

  int conn_fd = 0;
  while ((conn_fd = accept(server_fd, nullptr, nullptr)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      usleep(1000);
    } else {
      std::cerr << "接受连接时出错: " << strerror(errno) << std::endl;
      break;
    }
  }
  ASSERT_NE(conn_fd, -1);

  close(client_fd);  // 客户端断开连接

  ASSERT_THROW(selector.doSelect(conn_fd), std::runtime_error);
  close(conn_fd);
}

// 测试 socket 错误的情况
TEST_F(SelectorTest, SocketError) {
  int client_fd = createClient();

  int conn_fd = 0;
  while ((conn_fd = accept(server_fd, nullptr, nullptr)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      usleep(1000);
    } else {
      std::cerr << "接受连接时出错: " << strerror(errno) << std::endl;
      break;
    }
  }
  ASSERT_NE(conn_fd, -1);

  // 模拟 socket 错误
  shutdown(conn_fd, SHUT_RDWR);

  ASSERT_THROW(selector.doSelect(conn_fd), std::runtime_error);
  close(client_fd);
  close(conn_fd);
}
}  // namespace zipfiles::server