#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <iostream>
#include <stdexcept>
#include <libgen.h>
#include <unistd.h>
#include <functional>
#include <json-glib/json-glib.h>

static void sum_function(
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
  if (!jsc_value_is_number(timestamp)) {
    g_print("js_add_function: timestamp is not a number\n");
    g_object_unref(value);
    return;
  }
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  if (!jsc_value_is_string(apiEnum)) {
    g_print("js_add_function: apiEnum is not a string\n");
    g_object_unref(value);
    g_object_unref(timestamp);
    return;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  if (!jsc_value_is_object(params)) {
    g_print("js_add_function: params is not an object\n");
    g_object_unref(value);
    g_object_unref(timestamp);
    g_object_unref(apiEnum);
    return;
  }
  JSCValue* args = jsc_value_object_get_property(params, "args");
  if (!jsc_value_is_array(args)) {
    g_print("js_add_function: args is not an array\n");
    g_object_unref(value);
    g_object_unref(timestamp);
    g_object_unref(apiEnum);
    g_object_unref(params);
    return;
  }
  gchar** properties = nullptr;
  gchar* json_str = nullptr;
  char* script = nullptr;
  JsonBuilder* builder = nullptr;
  JsonGenerator* gen = nullptr;
  JsonNode* root = nullptr;
  try {
    double result = 0;
    properties = jsc_value_object_enumerate_properties(args);
    for (gchar** property = properties; *property; property++) {
      JSCValue* arg = jsc_value_object_get_property(args, *property);
      if (jsc_value_is_number(arg)) {
        result += jsc_value_to_double(arg);
      } else {
        g_object_unref(arg);
        throw std::invalid_argument("Arguments must be numbers");
      }
      g_object_unref(arg);
    }
    g_strfreev(properties);
    properties = nullptr;
    g_print("js_add_function: result = %f\n", result);
    const char* timestamp_str = jsc_value_to_string(timestamp);
    const char* apiEnum_str = jsc_value_to_string(apiEnum);

    builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "type");
    json_builder_add_string_value(builder, "resolve");
    json_builder_set_member_name(builder, "message");
    json_builder_add_string_value(builder, "Success");
    json_builder_set_member_name(builder, "timestamp");
    json_builder_add_string_value(builder, timestamp_str);
    json_builder_set_member_name(builder, "apiEnum");
    json_builder_add_string_value(builder, apiEnum_str);
    json_builder_set_member_name(builder, "data");
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "result");
    json_builder_add_double_value(builder, result);
    json_builder_end_object(builder);
    json_builder_end_object(builder);

    gen = json_generator_new();
    root = json_builder_get_root(builder);
    json_generator_set_root(gen, root);
    json_str = json_generator_to_data(gen, NULL);

    script = g_strdup_printf("window.postMessage(%s, '*');", json_str);

    WebKitWebView* webView = WEBKIT_WEB_VIEW(user_data);
    webkit_web_view_evaluate_javascript(
      webView, script, -1, NULL, NULL, NULL, NULL, NULL
    );
    g_free(script);
    g_free(json_str);
    g_object_unref(gen);
    json_node_free(root);
    g_object_unref(builder);
  } catch (const std::exception& e) {
    if (properties)
      g_strfreev(properties);
    if (json_str)
      g_free(json_str);
    if (script)
      g_free(script);
    if (gen)
      g_object_unref(gen);
    if (root)
      json_node_free(root);
    if (builder)
      g_object_unref(builder);

    const char* timestamp_str = timestamp ? jsc_value_to_string(timestamp) : "";
    const char* apiEnum_str = apiEnum ? jsc_value_to_string(apiEnum) : "";

    builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "type");
    json_builder_add_string_value(builder, "reject");
    json_builder_set_member_name(builder, "message");
    json_builder_add_string_value(builder, e.what());
    json_builder_set_member_name(builder, "timestamp");
    json_builder_add_string_value(builder, timestamp_str);
    json_builder_set_member_name(builder, "apiEnum");
    json_builder_add_string_value(builder, apiEnum_str);
    json_builder_set_member_name(builder, "data");
    json_builder_add_null_value(builder);
    json_builder_end_object(builder);

    gen = json_generator_new();
    root = json_builder_get_root(builder);
    json_generator_set_root(gen, root);
    json_str = json_generator_to_data(gen, NULL);

    script = g_strdup_printf("window.postMessage(%s, '*');", json_str);

    WebKitWebView* webView = WEBKIT_WEB_VIEW(user_data);
    webkit_web_view_evaluate_javascript(
      webView, script, -1, NULL, NULL, NULL, NULL, NULL
    );
    g_free(script);
    g_free(json_str);
    g_object_unref(gen);
    json_node_free(root);
    g_object_unref(builder);
  }

  g_object_unref(value);
  g_object_unref(timestamp);
  g_object_unref(apiEnum);
  g_object_unref(params);
  g_object_unref(args);
}

static void log_function(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  g_print("js_log_function called\n");
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!jsc_value_is_object(value)) {
    g_print("js_log_function: value is not an object\n");
    return;
  }
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  if (!jsc_value_is_number(timestamp)) {
    g_print("js_log_function: timestamp is not a number\n");
    g_object_unref(value);
    return;
  }
  JSCValue* apiEum = jsc_value_object_get_property(value, "apiEnum");
  if (!jsc_value_is_string(apiEum)) {
    g_print("js_log_function: apiEnum is not a string\n");
    g_object_unref(value);
    g_object_unref(timestamp);
    return;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  if (!jsc_value_is_object(params)) {
    g_print("js_log_function: params is not an object\n");
    g_object_unref(value);
    g_object_unref(timestamp);
    g_object_unref(apiEum);
    return;
  }
  JSCValue* message = jsc_value_object_get_property(params, "message");
  if (!jsc_value_is_string(message)) {
    g_print("js_log_function: message is not a string\n");
    g_object_unref(value);
    g_object_unref(timestamp);
    g_object_unref(apiEum);
    g_object_unref(params);
    return;
  }

  g_print("js:console.log: %s\n", jsc_value_to_string(message));

  const char* timestamp_str = jsc_value_to_string(timestamp);
  const char* message_str = jsc_value_to_string(message);

  JsonBuilder* builder = json_builder_new();
  json_builder_begin_object(builder);
  json_builder_set_member_name(builder, "type");
  json_builder_add_string_value(builder, "log");
  json_builder_set_member_name(builder, "timestamp");
  json_builder_add_string_value(builder, timestamp_str);
  json_builder_set_member_name(builder, "message");
  json_builder_add_string_value(builder, message_str);
  json_builder_end_object(builder);

  JsonGenerator* gen = json_generator_new();
  JsonNode* root = json_builder_get_root(builder);
  json_generator_set_root(gen, root);
  gchar* json_str = json_generator_to_data(gen, NULL);

  char* script = g_strdup_printf("window.postMessage(%s, '*');", json_str);

  WebKitWebView* webView = WEBKIT_WEB_VIEW(user_data);
  webkit_web_view_evaluate_javascript(
    webView, script, -1, NULL, NULL, NULL, NULL, NULL
  );
  g_free(script);
  g_free(json_str);
  g_object_unref(gen);
  json_node_free(root);
  g_object_unref(builder);

  g_object_unref(value);
  g_object_unref(timestamp);
  g_object_unref(apiEum);
  g_object_unref(params);
  g_object_unref(message);
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
  // 注册并连接 sum 函数
  webkit_user_content_manager_register_script_message_handler(manager, "sum");
  g_signal_connect(
    manager, "script-message-received::sum", G_CALLBACK(sum_function), web_view
  );

  // 注册并连接 log 函数
  webkit_user_content_manager_register_script_message_handler(manager, "log");
  g_signal_connect(
    manager, "script-message-received::log", G_CALLBACK(log_function), web_view
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