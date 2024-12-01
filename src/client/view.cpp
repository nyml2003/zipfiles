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

void handleNotify(const std::string& message, Code code) {
  sendResponse(Notification(message, code).toJson());
}

void handleNotify(const std::string& message) {
  sendResponse(Notification(message, Code::NOTIFICATION).toJson());
}

void handleError(
  const std::string& uuid,
  const std::string& message,
  Code code,
  std::optional<Api> api
) {
  if (api.has_value()) {
    sendResponse(
      Res(response::NoResponse({}), api.value(), uuid, code, message).toJson()
    );
    return;
  }
  sendResponse(
    Res(response::NoResponse({}), Api::NORESPONSE, uuid, code, message).toJson()
  );
}

void handleClientError(
  const std::string& uuid,
  const std::string& message,
  Api api
) {
  Json::Value root;
  root["api"] = static_cast<int>(api);
  root["uuid"] = uuid;
  root["code"] = static_cast<int>(Code::CLIENT_ERROR);
  root["message"] = message;
  sendResponse(root);
}

void SendLocalResponse(
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
  try {
    if (!isProcedureValid(value)) {
      handleNotify("Invalid procedure header", Code::NOTIFICATION);
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in getting message, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }

  log4cpp::Category::getRoot().infoStream() << "JS Log: " << message;
}

void handleProcedureError(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  try {
    if (!isProcedureValid(value)) {
      handleNotify("Invalid procedure header", Code::NOTIFICATION);
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in getting message, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }

  log4cpp::Category::getRoot().errorStream() << "JS Error: " << message;
}

void handleProcedureInfo(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  try {
    if (!isProcedureValid(value)) {
      handleNotify("Invalid procedure header", Code::NOTIFICATION);
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in getting message, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }

  log4cpp::Category::getRoot().infoStream() << "JS Info: " << message;
}

void handleProcedureWarn(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  try {
    if (!isProcedureValid(value)) {
      handleNotify("Invalid procedure header", Code::NOTIFICATION);
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in getting message, error: " + std::string(e.what()),
      Code::NOTIFICATION
    );
    return;
  }

  log4cpp::Category::getRoot().warnStream() << "JS Warn: " << message;
}

void handleFunction(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  JSCValue* uuid = jsc_value_object_get_property(value, "uuid");
  JSCValue* api = jsc_value_object_get_property(value, "api");
  JSCValue* request = jsc_value_object_get_property(value, "request");
  try {
    Json::Value req = jsc_value_to_string(request);
    log4cpp::Category::getRoot().infoStream()
      << "send request: " << req.asCString();
    Socket::getInstance().send(req.asCString());
  } catch (const std::exception& e) {
    handleError(
      jsc_value_to_string(uuid), "Failed in sending request", Code::ERROR,
      static_cast<Api>(jsc_value_to_int32(api))
    );
  }
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
    handleClientError(_uuid, "Failed in opening config", _api);
    return;
  }

  if (!reader.parse(in, root, false)) {
    handleClientError(_uuid, "Failed in parsing config", _api);
    return;
  }

  if (!root.isObject()) {
    return;
  }

  JSCValue* ip = jsc_value_object_get_property(request, "ip");
  if (ip && !jsc_value_is_null(ip) && jsc_value_is_undefined(ip)) {
    root["ip"] = jsc_value_to_string(ip);
  }
  JSCValue* defaultBackupPath =
    jsc_value_object_get_property(request, "defaultBackupPath");
  if (defaultBackupPath && !jsc_value_is_null(defaultBackupPath) &&
      !jsc_value_is_undefined(defaultBackupPath)) {
    root["defaultBackupPath"] = jsc_value_to_string(defaultBackupPath);
  }

  std::ofstream out(CONFIGURE_PATH);
  if (!out.is_open()) {
    handleClientError(_uuid, "Failed in opening config", _api);
    return;
  }
  out << root;
  out.close();
  Json::Value nullJson;
  SendLocalResponse(nullJson, _api, _uuid, Code::OK);
}

void handleReadConfig(
  WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
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
    handleClientError(_uuid, "Failed in opening config", _api);
    return;
  }

  if (!reader.parse(in, root, false)) {
    handleClientError(_uuid, "Failed in parsing config", _api);
    return;
  }

  if (!root.isObject()) {
    return;
  }

  Json::Value res;
  res["ip"] = root["ip"];
  res["defaultBackupPath"] = root["defaultBackupPath"];
  SendLocalResponse(res, _api, _uuid, Code::OK);
}

}  // namespace zipfiles::client