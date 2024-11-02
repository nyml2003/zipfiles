#include <gtest/gtest.h>
#include "client/socket.h"

namespace zipfiles::server {

int id = 0;

class QPS : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

void make_request() {
  for (; id < 100; id++) {
    Json::Value request = zipfiles::makeReqMockNeedTime(i)->toJson();
    request["timestamp"] = 0;
    request["uuid"] = "1234567890";
    std::cout << "Request: " << request << std::endl;
  }
}

void get_response() {
  client::Socket::getInstance().receive();
}

TEST_F(QPS, QPS) {  // NOLINT
}

}  // namespace zipfiles::server