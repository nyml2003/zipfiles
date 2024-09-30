#include <arpa/inet.h>
#include <json/json.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
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

void sendRequest(int sock) {
  int currentCounter = counter.fetch_add(1);
  std::cout << "Sending request with counter: " << currentCounter << std::endl;
  Json::Value request = zipfiles::makeReqMockNeedTime(currentCounter)->toJson();
  request["timestamp"] = 0;
  request["uuid"] = "1234567890";
  std::cout << "Request: " << request << std::endl;

  Json::StreamWriterBuilder writer;
  std::string requestData = Json::writeString(writer, request);

  if (send(sock, requestData.c_str(), requestData.size(), 0) < 0) {
    std::cerr << "Send failed" << std::endl;
    return;
  }

  std::array<char, 1024> buffer = {0};
  usedCounters.insert(currentCounter);
  ssize_t valread = read(sock, buffer.data(), 1024);
  if (valread < 0) {
    std::cerr << "Read failed" << std::endl;
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
      throw std::runtime_error("Counter not found");
    }
  }
}

int main() {
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
  try {
    for (int i = 0; i < 1000; i++) {
      std::thread([sock]() { sendRequest(sock); }).detach();
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    close(sock);
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    std::cout << "Used counters: ";
    for (const auto& counter : usedCounters) {
      std::cout << counter << " ";
    }
    std::cout << std::endl;
  }
  // 打印usedCounters

  return 0;
}