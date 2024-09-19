#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include "client/client.h"

int main(int argc, char* argv[]) {
  gtk_init(&argc, &argv);
  WebKitUserContentManager* manager = webkit_user_content_manager_new();
  WebKitWebView* web_view =
    WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(manager));
  zipfiles::client::client::loadDistUri(web_view);
  zipfiles::client::client::bindJS(manager, web_view);
  GtkWidget* window = zipfiles::client::client::createWindow(web_view);
  gtk_widget_show_all(window);
  gtk_main();
  return 0;
}