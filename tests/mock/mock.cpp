#include <arpa/inet.h>
#include <json/json.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include "mp/Request.h"

const int SERVER_PORT = 8080;
std::string SERVER_IP = "127.0.0.1";

std::mutex mtx;
std::atomic<int> counter(0);
std::set<int> usedCounters;
std::condition_variable cv;
bool allRequestsCompleted = false;

void sendRequest(int sock, int totalRequests) {
  int requestCount = 0;
  while (requestCount < totalRequests) {
    int currentCounter = counter.fetch_add(1);
    std::cout << "Sending request with counter: " << currentCounter
              << std::endl;
    Json::Value request =
      zipfiles::makeReqMockNeedTime(currentCounter)->toJson();
    request["timestamp"] = 0;
    request["uuid"] = "1234567890";
    std::cout << "Request: " << request << std::endl;

    Json::StreamWriterBuilder writer;
    std::string requestData = Json::writeString(writer, request);

    if (send(sock, requestData.c_str(), requestData.size(), 0) < 0) {
      std::cerr << "Send failed" << std::endl;
      return;
    }

    usedCounters.insert(currentCounter);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // 延迟20毫秒
    requestCount++;
  }

  {
    std::lock_guard<std::mutex> lock(mtx);
    allRequestsCompleted = true;
  }
  cv.notify_one();
}

void receiveResponse(int sock) {
  while (true) {
    std::array<char, 1024> buffer = {0};
    ssize_t valread = read(sock, buffer.data(), 1024);
    if (valread < 0) {
      std::cerr << "Read failed" << std::endl;
      break;
    } else {
      std::lock_guard<std::mutex> lock(mtx);
      Json::CharReaderBuilder reader;
      Json::Value response;
      std::string errors;
      std::istringstream is(std::string(buffer.data(), valread));
      Json::parseFromStream(reader, is, &response, &errors);

      int receivedCounter = response["payload"]["id"].asInt();
      std::cout << "Received response with counter: " << response << std::endl;
      auto it = usedCounters.find(receivedCounter);
      if (it != usedCounters.end()) {
        usedCounters.erase(it);
      } else {
        std::cout << "Used counters: ";
        for (const auto& counter : usedCounters) {
          std::cout << counter << " ";
        }
        std::cout << std::endl;
        throw std::runtime_error("Counter not found");
      }
    }
  }
}

int main() {
  try {
    int sock = 0;
    struct sockaddr_in serv_addr {};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      std::cerr << "Socket creation error" << std::endl;
      return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP.c_str(), &serv_addr.sin_addr) <= 0) {
      std::cerr << "Invalid address/ Address not supported" << std::endl;
      close(sock);
      return -1;
    }

    if (connect(sock, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
      std::cerr << "Connection Failed" << std::endl;
      close(sock);
      return -1;
    }

    int totalRequests = 10;  // 设置发送请求的总数
    std::thread requestThread(sendRequest, sock, totalRequests);
    std::thread responseThread(receiveResponse, sock);

    {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock, [] { return allRequestsCompleted; });
    }

    requestThread.join();
    // 不关闭套接字，让响应线程继续运行
    responseThread.join();  // 等待响应线程完成
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    std::cout << "Used counters: ";
    for (const auto& counter : usedCounters) {
      std::cout << counter << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}

{
  
}