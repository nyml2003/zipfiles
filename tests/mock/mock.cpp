#include <arpa/inet.h>
#include <json/json.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

std::mutex mtx;
std::atomic<int> counter(0);
std::set<int> usedCounters;

void sendRequest(int sock) {
  int currentCounter = counter.fetch_add(1);
  Json::Value request;
  request["apiEnum"] = 0;
  request["payload"]["path"] = "/app/gui/src";
  request["timestamp"] = currentCounter;
  request["uuid"] = "4293c2c8-89de-4f5e-bd5e-98cf3b1f5533";

  Json::StreamWriterBuilder writer;
  std::string requestData = Json::writeString(writer, request);

  if (send(sock, requestData.c_str(), requestData.size(), 0) < 0) {
    std::cerr << "Send failed" << std::endl;
    return;
  }

  std::array<char, 1024> buffer = {0};
  {
    std::lock_guard<std::mutex> lock(mtx);
    usedCounters.insert(currentCounter);
  }
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

    int receivedCounter = response["timestamp"].asInt();
    auto it = usedCounters.find(receivedCounter);
    if (it != usedCounters.end()) {
      usedCounters.erase(it);
    } else {
      throw std::runtime_error("Counter not found");
    }
  }
}

void createConnection() {
  int sock = 0;
  struct sockaddr_in serv_addr {};

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cerr << "Socket creation error" << std::endl;
    return;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);

  if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    close(sock);
    return;
  }

  if (connect(sock, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
    std::cerr << "Connection Failed" << std::endl;
    close(sock);
    return;
  }

  for (int i = 0; i < 10; i++) {
    std::thread([&sock] { sendRequest(sock); }).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  close(sock);
}

int main() {
  std::vector<std::thread> connections;
  for (int i = 0; i < 3; i++) {
    connections.emplace_back(createConnection);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  for (auto& conn : connections) {
    if (conn.joinable()) {
      conn.join();
    }
  }

  std::cout << "Used counters: ";
  for (const auto& counter : usedCounters) {
    std::cout << counter << " ";
  }
  std::cout << std::endl;

  return 0;
}
