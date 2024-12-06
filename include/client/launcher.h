#ifndef ZIPFILE_CLIENT_LAUNCHER_H
#define ZIPFILE_CLIENT_LAUNCHER_H
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <atomic>
#include <log4cpp/Category.hh>
#include <string>
#include <thread>

namespace zipfiles::client {
/**
 * @brief 文件路径长度
 *
 * @details 用于获取可执行文件路径, 一般不会超过1024
 *
 */
constexpr size_t EXE_PATH_SIZE = 1024;
/**
 * @brief 窗体宽度
 *
 */
constexpr int WINDOW_WIDTH = 800;
/**
 * @brief 窗体高度
 *
 */
constexpr int WINDOW_HEIGHT = 600;

// 配置文件地址
constexpr const char* CONFIGURE_PATH = "/usr/local/zipfiles/.zip/config.json";

/**
 * @brief 前端打包文件路径
 *
 */
constexpr const char* DIST_FILE = "/dist/index.html";

extern WebKitWebView* webView;
/**
 * @brief 消息处理器
 *
 * @details 消息名称和回调函数
 *
 */
struct Handler {
  std::string name;
  GCallback callback;
};

/**
 * @brief 加载前端页面
 *
 * @details 从可执行文件路径中获取前端打包文件路径并加载
 *
 */
void loadDistUri();
/**
 * @brief 绑定JS消息处理器
 *
 * @details 绑定JS消息处理器到WebView
 *
 * @param manager
 *
 */
void bindJS(WebKitUserContentManager* manager);
/**
 * @brief 创建窗体
 *
 * @details 创建一个窗体并添加WebView
 *
 * @return GtkWidget* 窗体指针
 *
 */
GtkWidget* createWindow();
/**
 * @brief 启动器
 *
 * @details 启动前端页面
 *
 */
class Launcher {
 public:
  Launcher() : isRunning(false) {}
  ~Launcher() { stop(); }
  Launcher(const Launcher&) = delete;
  Launcher& operator=(const Launcher&) = delete;
  Launcher(Launcher&&) = delete;
  Launcher& operator=(Launcher&&) = delete;
  /**
   * @brief 运行
   * @details 运行前端页面
   * @param argc
   * @param argv
   */
  void startGTK(int argc, char** argv);
  void startReciever();
  void startLogger();
  void stop();
  static Launcher& getInstance() {
    static Launcher instance;
    return instance;
  }
  std::atomic_bool isRunning;
  std::thread reciever;
};
}  // namespace zipfiles::client

#endif  // !ZIPFILE_CLIENT_CLIENT_H