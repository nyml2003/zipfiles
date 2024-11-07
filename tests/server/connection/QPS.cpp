#include <gtest/gtest.h>
#include <atomic>
#include <exception>
#include <mutex>
#include <thread>
#include <vector>
#include "client/socket.h"
#include "json/value.h"
#include "mp/Request.h"

namespace zipfiles {

std::atomic<int> id(0);   // 使用原子操作确保线程安全
std::mutex mutex;         // 全局互斥锁
std::map<int, bool> map;  // 用于跟踪请求是否收到响应
int max_request = 10000;

class QPS : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

void make_request() {
  while (true) {
    int current_id = id.fetch_add(1);  // 原子操作递增id
    if (current_id >= max_request) {
      break;
    }
    Json::Value request = zipfiles::Req(
                            zipfiles::request::MockNeedTime{current_id},
                            zipfiles::Api::MOCK_NEED_TIME, "1234567890"
    )
                            .toJson();
    Json::FastWriter writer;
    std::string json_str = writer.write(request);
    // std::cout << "Sending request: " << json_str << std::endl;
    client::Socket::getInstance().send(json_str);
  }
}

void get_response() {
  while (true) {
    std::vector<Json::Value> responses;
    try {
      client::Socket::getInstance().receive([&responses](const Json::Value& json
                                            ) {
        responses.push_back(json);
        // std::cout << "Received response: " << json.toStyledString()
        //           << std::endl;
      });
    } catch (const std::exception& e) {
      std::cout << __LINE__ << ": ";
      std::cout << e.what() << std::endl;
      break;
    }

    for (const auto& response : responses) {
      int response_id = response["payload"]["id"].asInt();

      std::lock_guard<std::mutex> lg(mutex);
      if (map.find(response_id) == map.end()) {
        map[response_id] = true;
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