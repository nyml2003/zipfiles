#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include "client/client.h"

void init(int argc, char* argv[]) {
  gtk_init(&argc, &argv);
  WebKitUserContentManager* manager = webkit_user_content_manager_new();
  WebKitWebView* web_view =
    WEBKIT_WEB_VIEW(webkit_web_view_new_with_user_content_manager(manager));
  zipfiles::client::client::load_dist_uri(web_view);
  zipfiles::client::client::bind_js_functions(manager, web_view);
  GtkWidget* window = zipfiles::client::client::create_window(web_view);
  gtk_widget_show_all(window);
  gtk_main();
}

int main(int argc, char* argv[]) {
  init(argc, argv);
  return 0;
}

// #include "mp/mp.h"
// #include "mp/GetFileList.h"
// #include <iostream>

// using namespace zipfiles::mp;

// int main() {
//   MessageQueue mq;

//   // 创建请求对象
//   Request request;
//   request.setApi(ApiType::GET_FILE_LIST);

//   // 发送请求
//   if (mq.sendRequest(request)) {
//     std::cout << "Request sent successfully." << std::endl;
//   } else {
//     std::cerr << "Failed to send request." << std::endl;
//   }

//   // 接收响应
//   Response response;
//   if (mq.receiveResponse(response)) {
//     if (response.is(StatusCode::OK)) {
//       GetFileListResponse getFileListResponse;
//       getFileListResponse.fromJson(response.getPayload());
//       std::vector<std::string> filenames =
//       getFileListResponse.getFilenames(); for (const auto& filename :
//       filenames) {
//         std::cout << filename << std::endl;
//       }
//     } else {
//       std::cerr << "Error in response." << std::endl;
//     }
//   } else {
//     std::cerr << "Failed to receive response." << std::endl;
//   }

//   return 0;
// }