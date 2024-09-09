#ifndef ZIPFILE_CLIENT_UI_H
#define ZIPFILE_CLIENT_UI_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include <functional>

namespace zipfiles::client::ui {

bool validate_request_header(JSCValue* value);

void send_response(
  WebKitWebView* webView,
  const char* type,
  const char* message,
  const char* timestamp_str,
  const char* apiEnum_str,
  JsonBuilder* builder
);
void handle_success(
  WebKitWebView* webView,
  JSCValue* value,
  std::function<void(JsonBuilder*)> build_data
);
void handle_error(
  WebKitWebView* webView,
  const std::exception& e,
  JSCValue* value
);
void sum(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);
void log(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

}  // namespace zipfile::client::ui

#endif  // !ZIPFILE_CLIENT_UI_H