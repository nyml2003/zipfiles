#ifndef ZIPFILE_CLIENT_WINDOW_H
#define ZIPFILE_CLIENT_WINDOW_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>

namespace zipfiles::client::window {
void load_dist_uri(WebKitWebView* webView);
void bind_js_functions(
  WebKitUserContentManager* manager,
  WebKitWebView* web_view
);
GtkWidget* create_window(WebKitWebView* web_view);

}  // namespace zipfiles::client::window

#endif  // !ZIPFILE_CLIENT_WINDOW_H