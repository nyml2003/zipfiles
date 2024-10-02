#ifndef ZIPFILE_CLIENT_VIEW_H
#define ZIPFILE_CLIENT_VIEW_H
#include <gtk/gtk.h>
#include <json/json.h>
#include <webkit2/webkit2.h>
#include <functional>
#include "mp/Request.h"
namespace zipfiles::client {
/**
 * @brief 检查函数（此处指有返回值的h5调用）请求头是否合法
 * @param value JS请求对象
 * @return 如果请求头不合法，返回false,否则返回true
 */
bool isFunctionValid(JSCValue* value);
/**
 * @brief 检查过程（此处指无返回值的h5调用）请求头是否合法
 * @param value JS请求对象
 * @return 如果请求头不合法，返回false,否则返回true
 */
bool isProcedureValid(JSCValue* value);
/**
 * @brief 发送响应
 * @details
 * 容器中的数据会被序列化为JSON字符串并发送到前端，可能是Result或者Error
 * @param root 将要发送的数据，类型为Json::Value
 */
void sendResponse(Json::Value& root);
/**
 * @brief 在root中构造JSON对象Result
 * @details 用于完成和h5的双向通信,成功分支
 * @param root
 * @param build_data 生成数据的回调函数
 */
void handleResult(
  Json::Value& root,
  const std::function<void(Json::Value&)>& build_data
);
/**
 * @brief 在root中构造JSON对象Error
 * @details 用于完成和h5的双向通信,失败分支
 * @param root
 * @param err 异常对象
 */
void handleError(const std::string& message);
/**
 * @brief 在root中构造JSON对象Notify
 * @details 容器主动通知h5的消息
 * @param root
 * @param build_data
 */
void handleNotify(const std::string& message);

/**
 * @brief 转发请求到具体的处理函数
 * @param manager
 * @param js_result
 * @param user_data
 */
void handleFunction(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void handleRemoteResponse(const ReqPtr& request);

// void handleLocalResponse(const ReqPtr& request);

void handleProcedureLog(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void handleProcedureWarn(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void handleProcedureInfo(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void handleProcedureError(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

}  // namespace zipfiles::client

#endif  // !ZIPFILE_CLIENT_VIEW_H