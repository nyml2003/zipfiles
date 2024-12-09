
#include "client/launcher.h"

#include "client/socket.h"
#include "client/view.h"

#include <libgen.h>
#include <csignal>
#include <iostream>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <string>
#include <thread>

namespace zipfiles::client {
WebKitWebView* webView = nullptr;
void loadDistUri() {
  std::array<char, EXE_PATH_SIZE> exe_path = {0};
  ssize_t len =
    readlink("/proc/self/exe", exe_path.data(), exe_path.size() - 1);
  if (len == -1) {
    log4cpp::Category::getRoot().errorStream() << "Failed to readlink";
    return;
  }
  exe_path.at(len) = '\0';
  char* dir_path = dirname(exe_path.data());
  std::string dist_path = std::string(dir_path) + DIST_FILE;
  webkit_web_view_load_uri(webView, ("file://" + dist_path).c_str());
}

void bindJS(WebKitUserContentManager* manager) {
  std::array<Handler, 8> handlers = {{
    {"function", reinterpret_cast<GCallback>(handleFunction)},
    {"log", reinterpret_cast<GCallback>(handleProcedureLog)},
    {"error", reinterpret_cast<GCallback>(handleProcedureError)},
    {"info", reinterpret_cast<GCallback>(handleProcedureInfo)},
    {"warn", reinterpret_cast<GCallback>(handleProcedureWarn)},
    {"updateConfig", reinterpret_cast<GCallback>(handleUpdateConfig)},
    {"readConfig", reinterpret_cast<GCallback>(handleReadConfig)},
    {"connect", reinterpret_cast<GCallback>(handleConnect)},
  }};

  for (const auto& handler : handlers) {
    webkit_user_content_manager_register_script_message_handler(
      manager, handler.name.c_str()
    );
    // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
    g_signal_connect(
      manager, ("script-message-received::" + handler.name).c_str(),
      handler.callback, webView
    );
  }
}

GtkWidget* createWindow() {
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));
  // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
  g_signal_connect(window, "destroy", G_CALLBACK(handleQuit), nullptr);
  return window;
}

void Launcher::startGTK(int argc, char** argv) {
  gtk_init(&argc, &argv);
  WebKitUserContentManager* manager = webkit_user_content_manager_new();
  webView =
    WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(manager));
  loadDistUri();
  bindJS(manager);
  GtkWidget* window = createWindow();
  gtk_widget_show_all(window);
}

/**
 * @brief
 * receiver线程的生命周期和socket的生命周期
 * receiver线程在gtk线程前启动
 * socket线程在gtk_widget_show_all(window);后实例化
 * receive函数应当在socket的状态为CONNECT时调用
 */
void Launcher::startReciever() {
  Launcher::getInstance().reciever = std::thread([this]() {
    log4cpp::Category::getRoot().debugStream() << "Reciever started";
    Socket::getInstance().initializeSocket();
    while (isRunning) {
      if (Socket::getInstance().active || Socket::getInstance().trigger) {
        Socket::getInstance().receive(sendResponse);
      }
    }
    log4cpp::Category::getRoot().debugStream() << "Reciever stopped";
  });
}

void Launcher::stop() {
  isRunning = false;
  raise(SIGINT);
}

void Launcher::startLogger() {
  try {
    // 假设配置文件名为 "log4cpp.properties"
    std::string configFilePath = "/app/config/client.log.properties";
    log4cpp::PropertyConfigurator::configure(configFilePath);
  } catch (log4cpp::ConfigureFailure& f) {
    std::cerr << "Configure Problem: " << f.what() << std::endl;
  }
}
}  // namespace zipfiles::client
