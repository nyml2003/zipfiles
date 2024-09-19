#ifndef ZIPFILE_CLIENT_CLIENT_H
#define ZIPFILE_CLIENT_CLIENT_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <string>

namespace zipfiles::client::client {

constexpr size_t EXE_PATH_SIZE = 1024;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr const char* DIST_FILE = "/dist/index.html";

struct Handler {
  std::string name;
  GCallback callback;
};
void loadDistUri(WebKitWebView* webView);
void bindJS(WebKitUserContentManager* manager, WebKitWebView* webView);
GtkWidget* createWindow(WebKitWebView* webView);
}  // namespace zipfiles::client::client

#endif  // !ZIPFILE_CLIENT_CLIENT_H