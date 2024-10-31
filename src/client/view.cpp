#include "client/view.h"
#include <future>
#include <log4cpp/Category.hh>
#include <thread>
#include "client/launcher.h"
#include "client/socket.h"
#include "jsc/jsc.h"
#include "json/value.h"
#include "mp/Request.h"
#include "mp/Response.h"
namespace zipfiles::client {
bool isFunctionValid(JSCValue* value) {
  /*
   * 检查函数是否合法
   * 1. value不为空
   * 2. value是一个对象
   * 3. value中包含timestamp, apiEnum, uuid, params
   * 4. timestamp是一个数字
   * 5. apiEnum是一个数字
   * 6. uuid是一个字符串
   * 7. params是一个字符串
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
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  if (jsc_value_is_number(timestamp) == 0) {
    log4cpp::Category::getRoot().errorStream()
      << __func__ << ": timestamp is not a number";
    return false;
  }
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  if (jsc_value_is_number(apiEnum) == 0) {
    log4cpp::Category::getRoot().errorStream()
      << __func__ << ": apiEnum is not a number";
    return false;
  }
  JSCValue* uuid = jsc_value_object_get_property(value, "uuid");
  if (jsc_value_is_string(uuid) == 0) {
    log4cpp::Category::getRoot().errorStream()
      << __func__ << ": uuid is not a string";
    return false;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  if (jsc_value_is_string(params) == 0) {
    log4cpp::Category::getRoot().errorStream()
      << __func__ << ": params is not an object";
    return false;
  }
  return true;
}

bool isProcedureValid(JSCValue* value) {
  /*
   * 检查过程是否合法
   * 1. value不为空
   * 2. value是一个对象
   * 3. value中包含params
   * 7. params是一个对象
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
  JSCValue* params = jsc_value_object_get_property(value, "params");
  if (jsc_value_is_object(params) == 0) {
    log4cpp::Category::getRoot().errorStream()
      << __func__ << ": params is not an object";
    return false;
  }
  return true;
}

void sendResponse(Json::Value& root) {
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

void handleResult(
  Json::Value& root,
  Json::Value& data,
  double timestamp,
  int apiEnum,
  const std::string& uuid
) {
  root["type"] = "resolve";
  root["message"] = "Success";
  root["timestamp"] = timestamp;
  root["apiEnum"] = apiEnum;
  root["data"] = data["payload"];
  root["uuid"] = uuid;
  sendResponse(root);
}

void handleError(const std::string& message) {
  Json::Value root;
  root["type"] = "reject";
  root["message"] = message;
  root["data"] = Json::nullValue;
  sendResponse(root);
}

void handleNotify(const std::string& message) {
  Json::Value root;
  root["type"] = "notify";
  root["message"] = message;
  root["data"] = Json::nullValue;
  sendResponse(root);
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
    message = jsc_value_to_string(jsc_value_object_get_property(
      jsc_value_object_get_property(value, "params"), "message"
    ));
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
    message = jsc_value_to_string(jsc_value_object_get_property(
      jsc_value_object_get_property(value, "params"), "message"
    ));
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
    message = jsc_value_to_string(jsc_value_object_get_property(
      jsc_value_object_get_property(value, "params"), "message"
    ));
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
    message = jsc_value_to_string(jsc_value_object_get_property(
      jsc_value_object_get_property(value, "params"), "message"
    ));
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
  try {
    if (!isFunctionValid(value)) {
      handleError("Invalid function header");
      return;
    }
  } catch (const std::exception& e) {
    handleError(
      "Failed in checking function header, error: " + std::string(e.what())
    );
    return;
  }

  ApiEnum api = ApiEnum::ERROR;
  try {
    api = static_cast<ApiEnum>(
      jsc_value_to_int32(jsc_value_object_get_property(value, "apiEnum"))
    );
    if (api == ApiEnum::ERROR) {
      handleError("Invalid apiEnum");
      return;
    }
  } catch (const std::exception& e) {
    handleError("Failed in getting apiEnum, error: " + std::string(e.what()));
    return;
  }
  Json::Reader reader;
  Json::Value params;
  try {
    reader.parse(
      jsc_value_to_string(jsc_value_object_get_property(value, "params")),
      params
    );
  } catch (const std::exception& e) {
    handleError("Failed in parsing params, error: " + std::string(e.what()));
    return;
  }
  log4cpp::Category::getRoot().infoStream()
    << "Received function call: " << params;
  Json::Value reqJson;
  reqJson["apiEnum"] = static_cast<int>(api);
  reqJson["payload"] = params;
  reqJson["timestamp"] =
    jsc_value_to_double(jsc_value_object_get_property(value, "timestamp"));
  reqJson["uuid"] =
    jsc_value_to_string(jsc_value_object_get_property(value, "uuid"));

  ReqPtr request = Req::fromJson(reqJson);
  log4cpp::Category::getRoot().infoStream()
    << "Sending request: " << request->toJson();
  try {
    handleRemoteResponse(request);
  } catch (const std::exception& e) {
    handleError("Failed to handle request: " + std::string(e.what()));
  }
}

void handleRemoteResponse(const ReqPtr& request) {
  auto future = std::async(
    std::launch::async,
    [](const ReqPtr& request) -> ResPtr {
      try {
        Socket::getInstance().send(request);
      } catch (const std::exception& e) {
        handleError("Failed to send request: " + std::string(e.what()));
        return nullptr;
      }
      ResPtr response = nullptr;
      try {
        response = Socket::getInstance().receive();
        if (response == nullptr) {
          handleError("Failed to receive response: response is null");
          return nullptr;
        }
      } catch (const std::exception& e) {
        handleError("Failed to receive response: " + std::string(e.what()));
        return nullptr;
      }
      return response;
    },
    request
  );

  std::thread([future = std::move(future)]() mutable {
    try {
      ResPtr response = future.get();
      if (response == nullptr) {
        return;
      }
      Json::Value res = response->toJson();
      Json::Value root;
      handleResult(
        root, res, res["timestamp"].asDouble(), res["apiEnum"].asInt(),
        res["uuid"].asString()
      );
    } catch (const std::exception& e) {
      handleError("Failed to handle request: " + std::string(e.what()));
    }
  }).detach();
}

}  // namespace zipfiles::client