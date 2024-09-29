#ifndef ZIPFILES_SERVER_TOOLS_THREADPOOL_H
#define ZIPFILES_SERVER_TOOLS_THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace zipfiles::server {
class ThreadPool {
 public:
  explicit ThreadPool(size_t numThreads);
  ~ThreadPool();

  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;

 private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  std::mutex queueMutex;
  std::condition_variable condition;
  std::atomic<bool> stop;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
  -> std::future<typename std::result_of<F(Args...)>::type> {
  using returnType = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<returnType()>>(
    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
  );

  std::future<returnType> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    if (stop) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }
    tasks.emplace([task] { (*task)(); });
  }
  condition.notify_one();
  return res;
}
}  // namespace zipfiles::server

#endif