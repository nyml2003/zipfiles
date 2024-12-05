#ifndef ZIPFILE_CLIENT_VIEW_H
#define ZIPFILE_CLIENT_VIEW_H

#include "mp/Notification.h"
#include "mp/common.h"

#include <JavaScriptCore/JavaScript.h>
#include <gtk/gtk.h>
#include <json/json.h>
#include <webkit2/webkit2.h>

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
 * 容器中的数据会被序列化为JSON字符串并发送到前端
 *
 * @param root 将要发送的数据，类型为Json::Value
 *
 */
void sendResponse(const Json::Value& root);

void sendLocalResponse(
  const Json::Value& root,
  Api api,
  const std::string& uuid,
  Code code
);

void handleClientError(
  const std::string& uuid,
  const std::string& title,
  const std::string& description
);

void handleNotify(const ZNotification& notification);

void handleNotifySingleSuccess(const std::string& message);

void handleNotifySingleError(const std::string& message);

void handleNotifySingleWarning(const std::string& message);

void handleNotifySingleInfo(const std::string& message);

void handleNotifyDoubleSuccess(
  const std::string& title,
  const std::string& description
);

void handleNotifyDoubleError(
  const std::string& title,
  const std::string& description
);

void handleNotifyDoubleWarning(
  const std::string& title,
  const std::string& description
);

void handleNotifyDoubleInfo(
  const std::string& title,
  const std::string& description
);
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

void handleUpdateConfig(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

void handleReadConfig(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

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

void handleQuit(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
);

}  // namespace zipfiles::client

#endif  // !ZIPFILE_CLIENT_VIEW_H