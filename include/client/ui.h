#ifndef ZIPFILE_CLIENT_UI_H
#define ZIPFILE_CLIENT_UI_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>

namespace zipfile::client {
struct RequestWrapper {
  JSCValue* timestamp;
  JSCValue* apiEnum;
};

void checkRequestWrapper(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
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

void buildSuccessJsonBegin(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void buildSuccessJsonEnd(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void buildErrorJsonBegin(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void buildErrorJsonEnd(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

}  // namespace zipfile::client

#endif  // !ZIPFILE_CLIENT_UI_H