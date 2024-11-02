#include <arpa/inet.h>
#include <json/json.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include "mp/Request.h"
#include "mp/mp.h"
#include "server/selector.h"

const int SERVER_PORT = 8080;
std::string SERVER_IP = "127.0.0.1";

std::mutex mtx;
std::atomic<int> counter(0);
std::set<int> usedCounters;
std::condition_variable cv;
bool allRequestsCompleted = false;

zipfiles::server::SelectorStatus state;
std::vector<uint8_t> header_buffer;
std::vector<uint8_t> write_buffer;
uint32_t data_size;

void readDataSize(uint8_t byte) {
  header_buffer.push_back(byte);

  if (header_buffer.size() == 4) {
    data_size = *reinterpret_cast<uint32_t*>(header_buffer.data());

    if (data_size == 0) {
      throw std::runtime_error("Illegal data_size, it is zero");
    }

    header_buffer.clear();
    state = zipfiles::server::SelectorStatus::READ_DATA;
  } else if (header_buffer.size() > 4) {
    throw std::runtime_error("Illegal header buffer size");
  }
}

bool readData(uint8_t byte) {
  write_buffer.push_back(byte);

  return data_size == write_buffer.size();
}

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
    uint32_t size = requestData.size();
    requestData =
      std::string(reinterpret_cast<char*>(&size), sizeof(size)) + requestData;

    if (send(sock, requestData.c_str(), requestData.size(), 0) < 0) {
      std::cerr << "Send failed" << std::endl;
      return;
    }

    usedCounters.insert(currentCounter);
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));  //
    // 延迟20毫秒
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
    std::vector<uint8_t> read_buffer;
    read_buffer.resize(zipfiles::mp::MAX_MESSAGE_SIZE);

    ssize_t bytesRead =
      read(sock, read_buffer.data(), zipfiles::mp::MAX_MESSAGE_SIZE);

    if (bytesRead > 0) {
      read_buffer.resize(bytesRead);  // 调整缓冲区大小以适应实际读取的数据
    }

    // 连接是否关闭
    if (bytesRead == 0) {
      // 断开连接
      close(sock);

      throw std::runtime_error(
        "Client " + std::to_string(sock) + " disconnect"
      );
    }

    // socket是否产生了错误
    if (bytesRead < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        close(sock);

        // 没有更多数据可读
        throw std::runtime_error(
          "Client " + std::to_string(sock) + "is broken, disconnect now"
        );
      }

      close(sock);
      // 未知错误
      throw std::runtime_error(
        "Client " + std::to_string(sock) +
        " is broken for unknown reason, disconnect now"
      );
    }

    for (const uint8_t byte : read_buffer) {
      switch (state) {
        case zipfiles::server::SelectorStatus::READ_DATA_SIZE:
          readDataSize(byte);
          break;
        case zipfiles::server::SelectorStatus::READ_DATA:
          if (readData(byte)) {
            Json::Reader reader;
            Json::Value response;
            std::string jsonString(write_buffer.begin(), write_buffer.end());

            reader.parse(jsonString, response);

            int receivedCounter = response["payload"]["id"].asInt();
            std::cout << "Received response with counter: " << response
                      << std::endl;
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

            state = zipfiles::server::SelectorStatus::READ_DATA_SIZE;
          }
          break;
        default:
          throw std::runtime_error("Unknown state");
      }
    }

    read_buffer.clear();
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

    if (connect(
          sock, reinterpret_cast<struct sockaddr*>(&serv_addr),
          sizeof(serv_addr)
        ) < 0) {
      std::cerr << "Connection Failed" << std::endl;
      close(sock);
      return -1;
    }

    int totalRequests = 10000;  // 设置发送请求的总数
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
