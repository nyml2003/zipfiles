
#include "client/view.h"
#include "client/client.h"
#include <iostream>
#include <string>
#include <libgen.h>

namespace zipfiles::client::client {

void loadDistUri(WebKitWebView* webView) {
  std::array<char, EXE_PATH_SIZE> exe_path = {0};
  ssize_t len =
    readlink("/proc/self/exe", exe_path.data(), exe_path.size() - 1);
  if (len == -1) {
    std::cerr << "无法获取可执行文件路径: " << strerror(errno) << std::endl;
    return;
  }
  exe_path.at(len) = '\0';
  char* dir_path = dirname(exe_path.data());
  std::string dist_path = std::string(dir_path) + DIST_FILE;
  webkit_web_view_load_uri(webView, ("file://" + dist_path).c_str());
}

void bindJS(WebKitUserContentManager* manager, WebKitWebView* webView) {
  std::array<Handler, 3> handlers = {
    {{"sum", G_CALLBACK(view::sum)},
     {"log", G_CALLBACK(view::log)},
     {"getFileList", G_CALLBACK(view::getFileList)}}};

  for (const auto& handler : handlers) {
    webkit_user_content_manager_register_script_message_handler(
      manager, handler.name.c_str()
    );
    g_signal_connect(
      manager, ("script-message-received::" + handler.name).c_str(),
      handler.callback, webView
    );
  }
}

GtkWidget* createWindow(WebKitWebView* webView) {
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webView));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  return window;
}

}  // namespace zipfiles::client::client