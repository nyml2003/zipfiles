#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <iostream>
#include <stdexcept>
#include <libgen.h>
#include <unistd.h>
#include <functional>

// 辅助函数：处理异常和参数校验
void handle_message(
  WebKitJavascriptResult* result,
  gpointer user_data,
  std::function<double(double, double)> operation,
  const char* success_callback,
  const char* error_callback
) {
  try {
    JSCValue* value = webkit_javascript_result_get_js_value(result);
    if (jsc_value_is_object(value)) {
      JSCValue* arg1 = jsc_value_object_get_property(value, "arg1");
      JSCValue* arg2 = jsc_value_object_get_property(value, "arg2");

      if (jsc_value_is_number(arg1) && jsc_value_is_number(arg2)) {
        double num1 = jsc_value_to_double(arg1);
        double num2 = jsc_value_to_double(arg2);
        g_print("num1: %f, num2: %f\n", num1, num2);
        double result = operation(num1, num2);
        g_print("result: %f\n", result);

        // 返回结果给 JavaScript
        WebKitWebView* web_view = WEBKIT_WEB_VIEW(user_data);
        char* script = g_strdup_printf(success_callback, result);
        webkit_web_view_evaluate_javascript(
          web_view, script, -1, NULL, NULL, NULL, NULL, NULL
        );
        g_free(script);
      } else {
        throw std::invalid_argument("Arguments must be numbers");
      }
    } else {
      throw std::invalid_argument("Invalid arguments");
    }
  } catch (const std::exception& e) {
    // 将错误信息传递给 JavaScript
    WebKitWebView* web_view = WEBKIT_WEB_VIEW(user_data);
    char* script = g_strdup_printf(error_callback, e.what());
    webkit_web_view_evaluate_javascript(
      web_view, script, -1, NULL, NULL, NULL, NULL, NULL
    );
    g_free(script);
  }
}

// C++ add 函数
static void add_function(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* result,
  gpointer user_data
) {
  handle_message(
    result, user_data, [](double num1, double num2) { return num1 + num2; },
    "window.postMessage({type: 'resolve', result: %f}, '*');",
    "window.postMessage({type: 'reject', error: '%s'}, '*');"
  );
}

// 日志处理函数
static void log_function(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* result,
  gpointer user_data
) {
  g_print("log_function called\n");
  JSCValue* value = webkit_javascript_result_get_js_value(result);
  if (jsc_value_is_string(value)) {
    const char* message = jsc_value_to_string(value);
    std::cout << "JavaScript log: " << message << std::endl;
  } else {
    g_print("log_function: value is not a string\n");
  }
}

// 辅助函数：注册和连接消息处理程序
void register_message_handler(
  WebKitUserContentManager* manager,
  const char* name,
  GCallback callback,
  gpointer user_data
) {
  webkit_user_content_manager_register_script_message_handler(manager, name);
  g_signal_connect(
    manager, ("script-message-received::" + std::string(name)).c_str(),
    callback, user_data
  );
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

int main(int argc, char* argv[]) {
  gtk_init(&argc, &argv);

  // 创建 WebView
  WebKitUserContentManager* manager = webkit_user_content_manager_new();
  WebKitWebView* web_view =
    WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(manager));

  // 使用辅助函数注册和连接消息处理程序
  register_message_handler(
    manager, "addFunction", G_CALLBACK(add_function), web_view
  );
  register_message_handler(manager, "log", G_CALLBACK(log_function), web_view);

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