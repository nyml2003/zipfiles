#ifndef ZIPFILE_CLIENT_VIEW_H
#define ZIPFILE_CLIENT_VIEW_H
#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <functional>
#include <json/json.h>
namespace zipfiles::client::view {
/**
 * @brief 检查请求头是否合法
 * @param value JS请求对象
 * @return true 合法 false 不合法，此时会抛出异常
 */
auto isRequestHeaderValid(JSCValue* value) -> bool;
/**
 * @brief 发送响应
 * @details
 * 容器中的数据会被序列化为JSON字符串并发送到前端，可能是Result或者Error
 * @param webView webview容器
 * @param root 将要发送的数据，类型为Json::Value
 */
void sendResponse(WebKitWebView* webView, Json::Value& root);
/**
 * @brief 处理成功
 * @details
 * 生成一个成功的响应并发送到前端
 * @param webView webview容器
 * @param value JS请求对象
 * @param build_data 生成数据的回调函数
 */
void handleSuccess(
  WebKitWebView* webView,
  JSCValue* value,
  const std::function<void(Json::Value&)>& build_data
);
/**
 * @brief 处理错误
 * @details
 * 生成一个错误的响应并发送到前端
 * @param webView webview容器
 * @param value JS请求对象
 * @param err 错误信息
 */
void handleError(
  WebKitWebView* webView,
  JSCValue* value,
  const std::exception& err
);
/**
 * @brief 求和
 * @details
 * 从JS请求对象中获取参数，求和并发送到前端
 * @param manager 用户内容管理器
 * @param js_result JS请求对象
 * @param user_data 用户数据
 */
void sum(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);
/**
 * @brief 记录
 * @details
 * 记录JS请求对象中的参数并发送到前端
 * @param manager 用户内容管理器
 * @param js_result JS请求对象
 * @param user_data 用户数据
 */
void log(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);
/**
 * @brief 获取文件列表
 * @details
 * 获取文件列表并发送到前端
 * @param manager 用户内容管理器
 * @param js_result JS请求对象
 * @param user_data 用户数据
 */
void getFileList(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);
/**
 * @brief 获取文件内容
 * @details
 * 获取文件内容并发送到前端
 * @param manager 用户内容管理器
 * @param js_result JS请求对象
 * @param user_data 用户数据
 */
void getFileDetail(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

}  // namespace zipfiles::client::view

#endif  // !ZIPFILE_CLIENT_VIEW_H