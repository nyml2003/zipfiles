#ifndef ZIPFILE_CLIENT_VIEW_H
#define ZIPFILE_CLIENT_VIEW_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include <functional>
namespace zipfiles::client::view {

bool validate_request_header(JSCValue* value);
void send_response(WebKitWebView* webView, JsonBuilder* builder);
void handle_success(
  WebKitWebView* webView,
  JSCValue* value,
  const std::function<void(JsonBuilder*)>& build_data
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
void getFileList(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

}  // namespace zipfiles::client::view

#endif  // !ZIPFILE_CLIENT_VIEW_H