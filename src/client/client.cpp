#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include "client/window.h"
#include "mq/MQWrapper.h"
#include "client/request.h"
#include "mq/GetFileList.h"

void init(int argc, char* argv[]) {
  gtk_init(&argc, &argv);
  WebKitUserContentManager* manager = webkit_user_content_manager_new();
  WebKitWebView* web_view =
    WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(manager));
  zipfiles::client::window::load_dist_uri(web_view);
  zipfiles::client::window::bind_js_functions(manager, web_view);
  GtkWidget* window = zipfiles::client::window::create_window(web_view);
  gtk_widget_show_all(window);
  gtk_main();
}

int main(int argc, char* argv[]) {
    //init(argc, argv);
    zipfiles::mq::MQWrapper mqWrapper;

  while (true) {
    int input;
    std::cout << "输入1以发送请求，输入其他值以退出: ";
    std::cin >> input;

    if (input != 1) {
      break;
    }

    zipfiles::client::request::Request<zipfiles::mq::GetFileList::GetFileList>
      request(zipfiles::mq::ApiType::GET_FILE_LIST, mqWrapper);

    zipfiles::mq::GetFileList::GetFileList content = request.handle();

    std::cout << content.to_string() << std::endl;
  }
  return 0;
}