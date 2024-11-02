#include <gtest/gtest.h>
#include <exception>
#include <mutex>
#include <thread>
#include "client/socket.h"
#include "json/value.h"

namespace zipfiles {

int id = 0;
std::mutex mutex;         // 全局互斥锁
std::map<int, bool> map;  // 用于跟踪请求是否收到响应
int max_request = 10000;

class QPS : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

void make_request() {
  for (; id < max_request; id++) {
    Json::Value request = zipfiles::makeReqMockNeedTime(id)->toJson();
    request["timestamp"] = 0;
    request["uuid"] = "1234567890";
    // std::cout << "Sending request: " << request.toStyledString() <<
    // std::endl;

    client::Socket::getInstance().send(Req::fromJson(request));
  }
}

void get_response() {
  while (true) {
    std::vector<ResPtr> responses;
    try {
      client::Socket::getInstance().receive(responses);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      break;
    }

    for (const auto& response : responses) {
      Json::Value resJson = response->toJson();
      int id = resJson["payload"]["id"].asInt();

      std::lock_guard<std::mutex> lg(mutex);
      if (map.find(id) == map.end()) {
        map[id] = true;
      }
    }

    if (map.size() == max_request) {
      break;
    }
  }
}

TEST_F(QPS, QPS) {  // NOLINT
  std::thread send_thread([]() { make_request(); });
  std::thread recv_thread([]() { get_response(); });

  auto start_time =
    std::chrono::high_resolution_clock::now();  // 记录测试开始时间

  send_thread.join();
  recv_thread.join();

  auto end_time =
    std::chrono::high_resolution_clock::now();  // 记录测试结束时间
  std::chrono::duration<double> test_duration = end_time - start_time;
  std::cout << "Total time for " << max_request
            << " requests: " << test_duration.count() << " seconds"
            << std::endl;

  // 计算QPS
  double qps = max_request / test_duration.count();
  std::cout << "QPS: " << qps << std::endl;
}
}  // namespace zipfiles