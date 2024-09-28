#include <future>
#include <log4cpp/Category.hh>
#include <thread>
#include "client/api.h"
#include "client/launcher.h"
#include "client/view.h"
#include "jsc/jsc.h"
#include "json/value.h"
#include "mp/Request.h"
#include "mp/Response.h"
namespace zipfiles::client::view {
bool isRequestHeaderValid(JSCValue* value) {
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
  if (jsc_value_is_number(apiEnum) == 0 && jsc_value_is_string(apiEnum) == 0) {
    log4cpp::Category::getRoot().errorStream()
      << __func__ << ": apiEnum is not a number or string";
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
  if (launcher::webView == nullptr) {
    return;
  }
  Json::StreamWriterBuilder writer;
  std::string json_str = Json::writeString(writer, root);
  std::string script = "window.postMessage(" + json_str + ", '*');";
  log4cpp::Category::getRoot().infoStream() << "evaluating script: " << script;
  webkit_web_view_evaluate_javascript(
    launcher::webView, script.c_str(), -1, nullptr, nullptr, nullptr, nullptr,
    nullptr
  );
}

void handleSuccess(
  Json::Value& root,
  const std::function<void(Json::Value&)>& build_data
) {
  root["type"] = "resolve";
  root["message"] = "Success";
  build_data(root);
  sendResponse(root);
}

void handleError(Json::Value& root, const std::exception& err) {
  root["type"] = "reject";
  root["message"] = err.what();
  root["data"] = Json::nullValue;
  sendResponse(root);
}

void handleFatal(const std ::exception& err) {
  Json::Value root;
  root["type"] = "fatal";
  root["message"] = err.what();
  root["timestamp"] = 0;
  root["apiEnum"] = "Fatal";
  root["data"] = Json::nullValue;
  log4cpp::Category::getRoot().errorStream() << "Fatal error: " << err.what();
  sendResponse(root);
}

void log(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isRequestHeaderValid(value)) {
    return;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  JSCValue* message = jsc_value_object_get_property(params, "message");
  Json::Value root;
  root["timestamp"] =
    jsc_value_to_double(jsc_value_object_get_property(value, "timestamp"));
  root["apiEnum"] =
    jsc_value_to_string(jsc_value_object_get_property(value, "apiEnum"));
  const char* message_str = jsc_value_to_string(message);
  log4cpp::Category::getRoot().infoStream() << "JS Log: " << message_str;
}

void handleMessage(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  [[maybe_unused]] gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isRequestHeaderValid(value)) {
    return;
  }
  try {
    JSCValue* params = jsc_value_object_get_property(value, "params");
    gdouble timestamp =
      jsc_value_to_double(jsc_value_object_get_property(value, "timestamp"));
    JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
    gint32 apiEnumInt = jsc_value_to_int32(apiEnum);
    auto api = static_cast<ApiEnum>(apiEnumInt);
    // todo 根据apiEnum调用不同的函数
    ReqPtr request = nullptr;
    switch (api) {
      case ApiEnum::GET_FILE_LIST: {
        // getFileList
        auto* path =
          jsc_value_to_string(jsc_value_object_get_property(params, "path"));
        request = makeReqGetFileList(path);
        break;
      }
      case ApiEnum::GET_FILE_DETAIL: {
        // getFileDetail
        auto* path =
          jsc_value_to_string(jsc_value_object_get_property(params, "path"));
        request = makeReqGetFileDetail(path);
        break;
      }
      default:
        throw std::invalid_argument("Invalid apiEnum");
        return;
    }
    if (request == nullptr) {
      throw std::runtime_error("Failed to create request");
    }
    request->timestamp = timestamp;
    log4cpp::Category::getRoot().infoStream()
      << "Sending request to server: " << request->toJson().toStyledString();
    // 异步调用 api::getFileList
    auto future = std::async(std::launch::async, doHandle, request);

    // 在后台线程中处理结果
    std::thread([future = std::move(future)]() mutable {
      try {
        ResPtr response = future.get();
        Json::Value res = response->toJson();
        int apiEnum = res["kind"].asInt();
        double timestamp = res["timestamp"].asDouble();
        Json::Value root;
        handleSuccess(root, [&](Json::Value& root) {
          root["data"] = response->toJson()["payload"];
          root["timestamp"] = timestamp;
          root["apiEnum"] = apiEnum;
        });
      } catch (const std::exception& e) {
        handleFatal(e);
      }
    }).detach();
  } catch (const std::exception& e) {
    handleFatal(e);
  }
}

}  // namespace zipfiles::client::view