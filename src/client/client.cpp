
#include "client/view.h"
#include "client/client.h"
#include <iostream>
#include <string>
#include <libgen.h>

namespace zipfiles::client::client {

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

void register_and_connect_handler(
  WebKitUserContentManager* manager,
  WebKitWebView* web_view,
  const char* handler_name,
  GCallback callback
) {
  webkit_user_content_manager_register_script_message_handler(
    manager, handler_name
  );
  g_signal_connect(
    manager, ("script-message-received::" + std::string(handler_name)).c_str(),
    callback, web_view
  );
}

void bind_js_functions(
  WebKitUserContentManager* manager,
  WebKitWebView* web_view
) {
  struct Handler {
    const char* name;
    GCallback callback;
  };

  Handler handlers[] = {
    {"sum", G_CALLBACK(view::sum)},
    {"log", G_CALLBACK(view::log)},
    {"getFileList", G_CALLBACK(view::getFileList)}};

  for (const auto& handler : handlers) {
    register_and_connect_handler(
      manager, web_view, handler.name, handler.callback
    );
  }
}

GtkWidget* create_window(WebKitWebView* web_view) {
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  return window;
}

}  // namespace zipfiles::client::client