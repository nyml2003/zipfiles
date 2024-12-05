#include "client/view.h"

#include <log4cpp/Category.hh>
#include "client/launcher.h"
#include "client/socket.h"
#include "json/value.h"
#include "mp/Notification.h"
#include "mp/Response.h"
#include "mp/common.h"

#include <fstream>

namespace zipfiles::client {
bool isProcedureValid(JSCValue* value) {
  /*
   * 检查过程是否合法
   * 1. value不为空
   * 2. value是一个对象
   */
  if (value == nullptr) {
    log4cpp::Category::getRoot().errorStream() << __func__ << ": value is null";
    return false;
  }
  if (jsc_value_is_object(value) == 0) {
    log4cpp::Category::getRoot().errorStream()
      << __func__ << ": value is not an object";
    return false;
  }
  return true;
}

void handleNotifySingleSuccess(const std::string& message) {
  sendResponse(ZNotification(
                 notification::SingleLine({.title = message}),
                 Code::SINGLE_SUCCESS
  )
                 .toJson());
}

void handleNotifySingleError(const std::string& message) {
  sendResponse(ZNotification(
                 notification::SingleLine({.title = message}),
                 Code::SINGLE_ERROR
  )
                 .toJson());
}

void handleNotifySingleWarning(const std::string& message) {
  sendResponse(ZNotification(
                 notification::SingleLine({.title = message}),
                 Code::SINGLE_WARNING
  )
                 .toJson());
}

void handleNotifySingleInfo(const std::string& message) {
  sendResponse(ZNotification(
                 notification::SingleLine({.title = message}), Code::SINGLE_INFO
  )
                 .toJson());
}

void handleNotifyDoubleSuccess(
  const std::string& title,
  const std::string& description
) {
  sendResponse(
    ZNotification(
      notification::DoubleLine({.title = title, .description = description}),
      Code::DOUBLE_SUCCESS
    )
      .toJson()
  );
}

void handleNotifyDoubleError(
  const std::string& title,
  const std::string& description
) {
  sendResponse(
    ZNotification(
      notification::DoubleLine({.title = title, .description = description}),
      Code::DOUBLE_ERROR
    )
      .toJson()
  );
}

void handleNotifyDoubleWarning(
  const std::string& title,
  const std::string& description
) {
  sendResponse(
    ZNotification(
      notification::DoubleLine({.title = title, .description = description}),
      Code::DOUBLE_WARNING
    )
      .toJson()
  );
}

void handleNotifyDoubleInfo(
  const std::string& title,
  const std::string& description
) {
  sendResponse(
    ZNotification(
      notification::DoubleLine({.title = title, .description = description}),
      Code::DOUBLE_INFO
    )
      .toJson()
  );
}

void handleNotify(const ZNotification& notification) {
  sendResponse(notification.toJson());
}

void handleClientError(
  const std::string& uuid,
  const std::string& title,
  const std::string& description
) {
  sendResponse(
    Res(
      response::NoResponse({.title = title, .description = description}), uuid,
      Code::CLIENT_ERROR
    )
      .toJson()
  );
}

void sendLocalResponse(
  const Json::Value& root,
  Api api,
  const std::string& uuid,
  Code code
) {
  Json::Value res;
  res["api"] = static_cast<int>(api);
  res["uuid"] = uuid;
  res["code"] = static_cast<int>(code);
  res["payload"] = root;
  sendResponse(res);
}

void sendResponse(const Json::Value& root) {
  if (webView == nullptr) {
    return;
  }
  Json::StreamWriterBuilder writer;
  std::string json_str = Json::writeString(writer, root);
  std::string script = "window.postMessage(" + json_str + ", '*');";
  log4cpp::Category::getRoot().infoStream() << "evaluating script: " << script;
  webkit_web_view_evaluate_javascript(
    webView, script.c_str(), -1, nullptr, nullptr, nullptr, nullptr, nullptr
  );
}

void handleProcedureLog(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isProcedureValid(value)) {
    handleNotifySingleError("log函数参数错误");
    return;
  }

  std::string message;

  message =
    jsc_value_to_string(jsc_value_object_get_property(value, "message"));

  log4cpp::Category::getRoot().infoStream() << "JS Log: " << message;
}

void handleProcedureError(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);

  if (!isProcedureValid(value)) {
    handleNotifySingleError("error函数参数错误");
    return;
  }

  std::string message;

  message =
    jsc_value_to_string(jsc_value_object_get_property(value, "message"));

  log4cpp::Category::getRoot().errorStream() << "JS Error: " << message;
}

void handleProcedureInfo(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isProcedureValid(value)) {
    handleNotifySingleError("info函数参数错误");
    return;
  }

  std::string message =
    jsc_value_to_string(jsc_value_object_get_property(value, "message"));

  log4cpp::Category::getRoot().infoStream() << "JS Info: " << message;
}

void handleProcedureWarn(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);

  if (!isProcedureValid(value)) {
    handleNotifySingleError("warn函数参数错误");
    return;
  }

  std::string message =
    jsc_value_to_string(jsc_value_object_get_property(value, "message"));

  log4cpp::Category::getRoot().warnStream() << "JS Warn: " << message;
}

void handleFunction(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  JSCValue* uuid = jsc_value_object_get_property(value, "uuid");
  JSCValue* request = jsc_value_object_get_property(value, "request");
  std::string req = jsc_value_to_string(request);
  std::string _uuid = jsc_value_to_string(uuid);

  std::thread t([req, _uuid] {
    try {
      log4cpp::Category::getRoot().debugStream() << "send request: " << req;
      Socket::getInstance().send(req);
    } catch (const std::exception& e) {
      handleClientError(_uuid, "请求失败", e.what());
    }
  });
  t.detach();
}

void handleUpdateConfig(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  JSCValue* uuid = jsc_value_object_get_property(value, "uuid");
  std::string _uuid = jsc_value_to_string(uuid);
  JSCValue* api = jsc_value_object_get_property(value, "api");
  Api _api = static_cast<Api>(jsc_value_to_int32(api));
  JSCValue* request = jsc_value_object_get_property(value, "request");
  Json::Reader reader;
  Json::Value root;
  // 读取CONFIGURE_PATH
  std::ifstream in(CONFIGURE_PATH);
  if (!in.is_open()) {
    handleClientError(_uuid, "更新配置失败", "无法打开配置文件");
    return;
  }

  if (!reader.parse(in, root, false)) {
    handleClientError(_uuid, "更新配置失败", "无法解析配置文件");
    return;
  }

  if (!root.isObject()) {
    return;
  }

  JSCValue* ip = jsc_value_object_get_property(request, "ip");
  JSCValue* defaultBackupPath =
    jsc_value_object_get_property(request, "defaultBackupPath");
  if (ip && !jsc_value_is_null(ip) && !jsc_value_is_undefined(ip)) {
    root["ip"] = jsc_value_to_string(ip);
  }

  if (defaultBackupPath && !jsc_value_is_null(defaultBackupPath) &&
      !jsc_value_is_undefined(defaultBackupPath)) {
    root["defaultBackupPath"] = jsc_value_to_string(defaultBackupPath);
  }
  log4cpp::Category::getRoot().infoStream() << "update config: " << root;
  std::ofstream out(CONFIGURE_PATH);
  if (!out.is_open()) {
    handleClientError(_uuid, "更新配置失败", "无法打开配置文件");
    return;
  }
  out << root;
  out.close();
  Json::Value nullJson;
  sendLocalResponse(nullJson, _api, _uuid, Code::OK);
}

void handleReadConfig(
  WebKitUserContentManager* /*manager*/,
  WebKitJavascriptResult* js_result,
  gpointer /*user_data*/
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  JSCValue* uuid = jsc_value_object_get_property(value, "uuid");
  std::string _uuid = jsc_value_to_string(uuid);
  JSCValue* api = jsc_value_object_get_property(value, "api");
  Api _api = static_cast<Api>(jsc_value_to_int32(api));
  Json::Reader reader;
  Json::Value root;
  // 读取CONFIGURE_PATH
  std::ifstream in(CONFIGURE_PATH);
  if (!in.is_open()) {
    Json::Value defaultConfig;
    defaultConfig["ip"] = "127.0.0.1";
    defaultConfig["defaultBackupPath"] = "/usr/local/zipfiles";
    defaultConfig["port"] = 8080;
    defaultConfig["version"] = "1.0.0";
    // 创建文件
    std::ofstream out(CONFIGURE_PATH);
    if (!out.is_open()) {
      handleClientError(_uuid, "读取配置失败", "无法打开配置文件");
    }
    out << defaultConfig;
    out.close();
    in.open(CONFIGURE_PATH);
  }

  if (!reader.parse(in, root, false)) {
    handleClientError(_uuid, "读取配置失败", "无法解析配置文件");
    return;
  }

  if (!root.isObject()) {
    return;
  }

  Json::Value res;
  res["ip"] = root["ip"];
  res["defaultBackupPath"] = root["defaultBackupPath"];
  res["port"] = root["port"];
  res["version"] = root["version"];
  sendLocalResponse(res, _api, _uuid, Code::OK);
}

void handleQuit(
  WebKitUserContentManager* /*manager*/,
  WebKitJavascriptResult* /*js_result*/,
  gpointer /*user_data*/
) {
  gtk_main_quit();
  Launcher::getInstance().stop();
}

}  // namespace zipfiles::client