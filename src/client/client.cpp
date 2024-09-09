#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <iostream>
#include <stdexcept>
#include <libgen.h>
#include <unistd.h>
#include <functional>

static void js_add_function(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  g_print("js_add_function called\n");
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!jsc_value_is_object(value)) {
    g_print("js_add_function: value is not an object\n");
    return;
  }
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  JSCValue* data = jsc_value_object_get_property(value, "data");
  if (!jsc_value_is_object(data)) {
    g_print("js_add_function: data is not an object\n");
    return;
  }
  // 输出timestamp和apiEnum
    g_print("js_add_function: timestamp = %s\n", jsc_value_to_string(timestamp));
    g_print("js_add_function: apiEnum = %s\n", jsc_value_to_string(apiEnum));
  JSCValue* args = jsc_value_object_get_property(data, "args");
  if (!jsc_value_is_array(args)) {
    g_print("js_add_function: args is not an array\n");
    return;
  }
  try {
    unsigned length = jsc_value_typed_array_get_length(args);
    g_print("js_add_function: args length = %u\n", length);
    double result = 0;
    for (unsigned i = 0; i < length; i++) {
      JSCValue* arg = jsc_value_object_get_property_at_index(args, i);
      if (jsc_value_is_number(arg)) {
        result += jsc_value_to_double(arg);
      } else {
        throw std::invalid_argument("Arguments must be numbers");
      }
    }
    g_print("js_add_function: result = %f\n", result);
    const char* timestamp_str = jsc_value_to_string(timestamp);
    const char* apiEnum_str = jsc_value_to_string(apiEnum);
    char* script = g_strdup_printf(
      "window.postMessage({"
      "type: 'resolve',"
      "message: 'Success',"
      "timestamp: '%s',"
      "apiEnum: '%s',"
      "data: { result: %f }"
      "}, '*');",
      timestamp_str, apiEnum_str, result
    );
    WebKitWebView* webView = WEBKIT_WEB_VIEW(user_data);
    webkit_web_view_evaluate_javascript(
      webView, script, -1, NULL, NULL, NULL, NULL, NULL
    );
    g_free(script);
  } catch (const std::exception& e) {
    const char* timestamp_str = timestamp ? jsc_value_to_string(timestamp) : "";
    const char* apiEnum_str = apiEnum ? jsc_value_to_string(apiEnum) : "";
    WebKitWebView* webView = WEBKIT_WEB_VIEW(user_data);
    char* script = g_strdup_printf(
      "window.postMessage({"
      "type: 'reject',"
      "message: '%s',"
      "timestamp: '%s',"
      "apiEnum: '%s',"
      "data: null"
      "}, '*');",
      e.what(), timestamp_str, apiEnum_str
    );
    webkit_web_view_evaluate_javascript(
      webView, script, -1, NULL, NULL, NULL, NULL, NULL
    );
    g_free(script);
  }
}

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
  webkit_user_content_manager_register_script_message_handler(
    manager, "js_add_function"
  );
  g_signal_connect(
    manager, "script-message-received::js_add_function",
    G_CALLBACK(js_add_function), web_view
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