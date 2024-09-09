#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <iostream>
#include <stdexcept>
#include <libgen.h>
#include <unistd.h>
#include <functional>
#include <json-glib/json-glib.h>
#include "client/ui.h"

void load_dist_uri(WebKitWebView* web_view) {
  // 获取当前可执行文件的路径
  char exe_path[1024];
  ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
  if (len == -1) {
    std::cerr << "无法获取可执行文件路径" << std::endl;
    return;
  }
  exe_path[len] = '\0';

  // 获取目录部分
  char* dir_path = dirname(exe_path);

  // 构建指向 dist 文件夹的路径
  std::string dist_path = std::string(dir_path) + "/dist/index.html";

  // 加载新的 URI
  webkit_web_view_load_uri(web_view, ("file://" + dist_path).c_str());
}

static void
bind_js_functions(WebKitUserContentManager* manager, WebKitWebView* web_view) {
  // 注册并连接 sum 函数
  webkit_user_content_manager_register_script_message_handler(manager, "sum");
  g_signal_connect(
    manager, "script-message-received::sum", G_CALLBACK(zipfiles::client::ui::sum),
    web_view
  );

  // 注册并连接 log 函数
  webkit_user_content_manager_register_script_message_handler(manager, "log");
  g_signal_connect(
    manager, "script-message-received::log", G_CALLBACK(zipfiles::client::ui::log),
    web_view
  );
}

int main(int argc, char* argv[]) {
  gtk_init(&argc, &argv);

  // 创建 WebView
  WebKitUserContentManager* manager = webkit_user_content_manager_new();
  WebKitWebView* web_view =
    WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(manager));

  bind_js_functions(manager, web_view);
  // 加载本地 HTML 文件
  load_dist_uri(web_view);

  // 创建 GTK 窗口
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(
    GTK_WINDOW(window), 800, 600
  );  // 设置窗口大小为 800x600
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}