#include "client/ui.h"
#include <iostream>
namespace zipfiles::client::ui {
bool validate_request_header(JSCValue* value) {
  if (!jsc_value_is_object(value)) {
    g_print("js_add_function: value is not an object\n");
    return false;
  }
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  if (!jsc_value_is_number(timestamp)) {
    g_print("js_add_function: timestamp is not a number\n");
    return false;
  }
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  if (!jsc_value_is_string(apiEnum)) {
    g_print("js_add_function: apiEnum is not a string\n");
    return false;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  if (!jsc_value_is_object(params)) {
    g_print("js_add_function: params is not an object\n");
    return false;
  }
  return true;
}

void send_response(
  WebKitWebView* webView,
  const char* type,
  const char* message,
  const char* timestamp_str,
  const char* apiEnum_str,
  JsonBuilder* builder
) {
  JsonGenerator* gen = json_generator_new();
  JsonNode* root = json_builder_get_root(builder);
  json_generator_set_root(gen, root);
  gchar* json_str = json_generator_to_data(gen, NULL);
  char* script = g_strdup_printf("window.postMessage(%s, '*');", json_str);

  webkit_web_view_evaluate_javascript(
    webView, script, -1, NULL, NULL, NULL, NULL, NULL
  );

  g_free(script);
  g_free(json_str);
  g_object_unref(gen);
  json_node_free(root);
  g_object_unref(builder);
}

void handle_success(
  WebKitWebView* webView,
  JSCValue* value,
  std::function<void(JsonBuilder*)> build_data
) {
  JsonBuilder* builder = json_builder_new();
  json_builder_begin_object(builder);
  json_builder_set_member_name(builder, "type");
  json_builder_add_string_value(builder, "resolve");
  json_builder_set_member_name(builder, "message");
  json_builder_add_string_value(builder, "Success");
  json_builder_set_member_name(builder, "timestamp");
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  const char* timestamp_str = jsc_value_to_string(timestamp);
  json_builder_add_string_value(builder, timestamp_str);
  json_builder_set_member_name(builder, "apiEnum");
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  const char* apiEnum_str = jsc_value_to_string(apiEnum);
  json_builder_add_string_value(builder, apiEnum_str);
  json_builder_set_member_name(builder, "data");
  json_builder_begin_object(builder);
  build_data(builder);
  json_builder_end_object(builder);
  json_builder_end_object(builder);

  send_response(
    webView, "resolve", "Success", timestamp_str, apiEnum_str, builder
  );
}

void handle_error(
  WebKitWebView* webView,
  const std::exception& e,
  JSCValue* value
) {
  JsonBuilder* builder = json_builder_new();
  json_builder_begin_object(builder);
  json_builder_set_member_name(builder, "type");
  json_builder_add_string_value(builder, "reject");
  json_builder_set_member_name(builder, "message");
  json_builder_add_string_value(builder, e.what());
  json_builder_set_member_name(builder, "timestamp");
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  const char* timestamp_str = jsc_value_to_string(timestamp);
  json_builder_add_string_value(builder, timestamp_str);
  json_builder_set_member_name(builder, "apiEnum");
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  const char* apiEnum_str = jsc_value_to_string(apiEnum);
  json_builder_add_string_value(builder, apiEnum_str);
  json_builder_set_member_name(builder, "data");
  json_builder_add_null_value(builder);
  json_builder_end_object(builder);

  send_response(
    webView, "reject", e.what(), timestamp_str, apiEnum_str, builder
  );
}

void sum(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!validate_request_header(value)) {
    return;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  JSCValue* args = jsc_value_object_get_property(params, "args");
  try {
    double result = 0;
    gchar** properties = jsc_value_object_enumerate_properties(args);
    for (gchar** property = properties; *property; property++) {
      JSCValue* arg = jsc_value_object_get_property(args, *property);
      if (jsc_value_is_number(arg)) {
        result += jsc_value_to_double(arg);
      } else {
        g_strfreev(properties);
        throw std::invalid_argument("Arguments must be numbers");
      }
    }
    g_strfreev(properties);
    handle_success(
      WEBKIT_WEB_VIEW(user_data), value,
      [&](JsonBuilder* builder) {
        json_builder_set_member_name(builder, "result");
        json_builder_add_double_value(builder, result);
      }
    );
  } catch (const std::exception& e) {
    handle_error(WEBKIT_WEB_VIEW(user_data), e, value);
  }
}

void log(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!validate_request_header(value)) {
    return;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  JSCValue* message = jsc_value_object_get_property(params, "message");
  const char* message_str = jsc_value_to_string(message);
  g_print("JS Log: %s\n", message_str);
  handle_success(WEBKIT_WEB_VIEW(user_data), value, [&](JsonBuilder* builder) {
    json_builder_set_member_name(builder, "message");
    json_builder_add_string_value(builder, message_str);
  });
}
}  // namespace zipfiles::client::ui