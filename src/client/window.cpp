
#include "client/window.h"
#include "client/ui.h"
#include <iostream>
#include <libgen.h>

namespace zipfiles::client::window {

void load_dist_uri(WebKitWebView* web_view) {
  char exe_path[1024];
  ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
  if (len == -1) {
    std::cerr << "无法获取可执行文件路径" << std::endl;
    return;
  }
  exe_path[len] = '\0';
  char* dir_path = dirname(exe_path);
  std::string dist_path = std::string(dir_path) + "/dist/index.html";
  webkit_web_view_load_uri(web_view, ("file://" + dist_path).c_str());
}

void bind_js_functions(
  WebKitUserContentManager* manager,
  WebKitWebView* web_view
) {
  webkit_user_content_manager_register_script_message_handler(manager, "sum");
  g_signal_connect(
    manager, "script-message-received::sum",
    G_CALLBACK(zipfiles::client::ui::sum), web_view
  );
  webkit_user_content_manager_register_script_message_handler(manager, "log");
  g_signal_connect(
    manager, "script-message-received::log",
    G_CALLBACK(zipfiles::client::ui::log), web_view
  );
}

GtkWidget* create_window(WebKitWebView* web_view) {
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  return window;
}

}  // namespace zipfiles::client::window