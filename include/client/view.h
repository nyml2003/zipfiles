#ifndef ZIPFILE_CLIENT_VIEW_H
#define ZIPFILE_CLIENT_VIEW_H
#include <gtk/gtk.h>
#include <json/json.h>
#include <webkit2/webkit2.h>
#include <functional>
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
 * @param root 将要发送的数据，类型为Json::Value
 */
void sendResponse(Json::Value& root);
/**
 * @brief 处理成功
 * @details
 * 生成一个成功的响应并发送到前端
 * @param value JS请求对象
 * @param build_data 生成数据的回调函数
 */
void handleSuccess(
  Json::Value& root,
  const std::function<void(Json::Value&)>& build_data
);
/**
 * @brief 处理错误
 * @details
 * 生成一个错误的响应并发送到前端
 * @param value JS请求对象
 * @param err 错误信息
 */
void handleError(Json::Value& root, const std::exception& err);
/**
 * @brief 处理致命错误
 * 列举如下：
 * - 和后端通信失败
 * @param value
 * @param err
 */
void handleFatal(const std::exception& err);
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
void handleMessage(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);
}  // namespace zipfiles::client::view

#endif  // !ZIPFILE_CLIENT_VIEW_H