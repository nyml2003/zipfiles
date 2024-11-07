#ifndef ZIPFILE_CLIENT_VIEW_H
#define ZIPFILE_CLIENT_VIEW_H
#include <JavaScriptCore/JavaScript.h>
#include <gtk/gtk.h>
#include <json/json.h>
#include <webkit2/webkit2.h>
#include "mp/common.h"
namespace zipfiles::client {
/**
 * @brief 检查过程（此处指无返回值的h5调用）请求头是否合法
 *
 * @param value JS请求对象
 *
 * @return 如果请求头不合法，返回false,否则返回true
 *
 */
bool isProcedureValid(JSCValue* value);
/**
 * @brief 发送响应
 *
 * @details
 * 容器中的数据会被序列化为JSON字符串并发送到前端，可能是Result或者Error
 *
 * @param root 将要发送的数据，类型为Json::Value
 *
 */
void sendResponse(const Json::Value& root);

/**
 * @brief 处理客户端的请求失败
 *
 * @param uuid 请求的uuid
 * @param api 请求的api
 * @param message 错误信息
 * @param code 默认为Code::CLIENT_ERROR, 而且一般不应该是其他值
 */
void handleError(
  const std::string& uuid,
  const std::string& api,
  const std::string& message,
  Code code
);
/**
 * @brief 处理客户端主动向前端发送的消息
 *
 * @param message
 * @param code 默认为Code::NOTIFICATION, 而且一般不应该是其他值
 */
void handleNotify(const std::string& message);

/**
 * @brief 转发请求到具体的处理函数
 *
 * @param manager
 *
 * @param js_result
 *
 * @param user_data
 *
 */
void handleFunction(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void handleRemoteResponse(Json::Value response);

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