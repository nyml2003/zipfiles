#include <arpa/inet.h>
#include <json/json.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define REQUESTS_PER_SECOND 1000

std::mutex mtx;

void sendRequest() {
  int sock = 0;
  struct sockaddr_in serv_addr;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection Failed" << std::endl;
    close(sock);
    return;
  }

  Json::Value request;
  request["apiEnum"] = 1;
  request["payload"]["path"] = "/app/gui/src";
  request["timestamp"] = 1727629169132.0;
  request["uuid"] = "4293c2c8-89de-4f5e-bd5e-98cf3b1f5533";
  Json::StreamWriterBuilder writer;
  std::string requestData = Json::writeString(writer, request);

  if (send(sock, requestData.c_str(), requestData.size(), 0) < 0) {
    std::cerr << "Send failed" << std::endl;
    close(sock);
    return;
  }

  char buffer[1024] = {0};
  int valread = read(sock, buffer, 1024);
  if (valread < 0) {
    std::cerr << "Read failed" << std::endl;
  } else {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Response received: " << std::string(buffer, valread)
              << std::endl;
  }

  close(sock);
}

int main() {
  while (true) {
    std::vector<std::thread> threads;

    for (int i = 0; i < REQUESTS_PER_SECOND; ++i) {
      threads.emplace_back(sendRequest);
    }

    for (auto& th : threads) {
      th.join();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
