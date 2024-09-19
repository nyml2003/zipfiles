#ifndef ZIPFILE_CLIENT_VIEW_H
#define ZIPFILE_CLIENT_VIEW_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <functional>
#include <json/json.h>
namespace zipfiles::client::view {

auto isRequestHeaderValid(JSCValue* value) -> bool;
void sendResponse(WebKitWebView* webView, Json::Value& root);
void handleSuccess(
  WebKitWebView* webView,
  JSCValue* value,
  const std::function<void(Json::Value&)>& build_data
);
void handleError(
  WebKitWebView* webView,
  JSCValue* value,
  const std::exception& err
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