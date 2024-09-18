#ifndef ZIPFILE_CLIENT_CLIENT_H
#define ZIPFILE_CLIENT_CLIENT_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>

namespace zipfiles::client::client {
void load_dist_uri(WebKitWebView* webView);
void bind_js_functions(
  WebKitUserContentManager* manager,
  WebKitWebView* web_view
);
GtkWidget* create_window(WebKitWebView* web_view);
void register_and_connect_handler(
  WebKitUserContentManager* manager,
  WebKitWebView* web_view,
  const char* handler_name,
  GCallback callback
);
}  // namespace zipfiles::client::client

#endif  // !ZIPFILE_CLIENT_CLIENT_H