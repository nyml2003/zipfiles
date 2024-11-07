#include "client/view.h"

#include <log4cpp/Category.hh>
#include "client/launcher.h"
#include "client/socket.h"
#include "json/value.h"
#include "mp/common.h"
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

void handleError(
  const std::string& uuid,
  const std::string& api,
  const std::string& message,
  Code code = Code::CLIENT_ERROR
) {
  Json::Value res;
  res["message"] = message;
  res["code"] = static_cast<int>(code);
  res["uuid"] = uuid;
  res["api"] = api;
  sendResponse(res);
}
/**
 * @brief 处理客户端主动向前端发送的消息
 *
 * @param message
 * @param code 默认为Code::NOTIFICATION, 而且一般不应该是其他值
 */
void handleNotify(const std::string& message) {
  Json::Value res;
  res["message"] = message;
  res["code"] = static_cast<int>(Code::NOTIFICATION);
  sendResponse(res);
}

void sendResponse(const Json::Value& root) {
  if (webView == nullptr) {
    return;
  }
  Json::StreamWriterBuilder writer;
  std::string json_str = Json::writeString(writer, root);
  std::string script = "window.postMessage(" + json_str + ", '*');";
  // log4cpp::Category::getRoot().infoStream() << "evaluating script: " <<
  // script;
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
      handleNotify("Invalid procedure header");
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what())
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify("Failed in getting message, error: " + std::string(e.what()));
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
      handleNotify("Invalid procedure header");
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what())
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify("Failed in getting message, error: " + std::string(e.what()));
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
      handleNotify("Invalid procedure header");
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what())
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify("Failed in getting message, error: " + std::string(e.what()));
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
      handleNotify("Invalid procedure header");
      return;
    }
  } catch (const std::exception& e) {
    handleNotify(
      "Failed in checking procedure header, error: " + std::string(e.what())
    );
    return;
  }
  std::string message;
  try {
    message =
      jsc_value_to_string(jsc_value_object_get_property(value, "message"));
  } catch (const std::exception& e) {
    handleNotify("Failed in getting message, error: " + std::string(e.what()));
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
      jsc_value_to_string(uuid), jsc_value_to_string(api),
      "Failed to send request: " + std::string(e.what())
    );
  }
}

void handleRemoteResponse(Json::Value res) {
  log4cpp::Category::getRoot().infoStream()
    << "received response: " << res.toStyledString();
  if (res["code"].asInt() == static_cast<int>(Code::NOTIFICATION)) {
    handleNotify(res["message"].asString());
  } else if (res["code"].asInt() == static_cast<int>(Code::OK)) {
    sendResponse(res);
  } else {
    handleError(
      res["uuid"].asString(), res["api"].asString(), res["message"].asString(),
      static_cast<Code>(res["code"].asInt())
    );
  }
}

}  // namespace zipfiles::client