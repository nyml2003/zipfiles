#include "client/view.h"
#include "client/api.h"
#include "utils.h"

namespace zipfiles::client::view {
bool isRequestHeaderValid(JSCValue* value) {
  if (jsc_value_is_object(value) == 0) {
    g_print("js_add_function: value is not an object\n");  // NOLINT
    return false;
  }
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  if (jsc_value_is_number(timestamp) == 0) {
    g_print("js_add_function: timestamp is not a number\n");  // NOLINT
    return false;
  }
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  if (jsc_value_is_string(apiEnum) == 0) {
    g_print("js_add_function: apiEnum is not a string\n");  // NOLINT
    return false;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  if (jsc_value_is_object(params) == 0) {
    g_print("js_add_function: params is not an object\n");  // NOLINT
    return false;
  }
  return true;
}

void sendResponse(WebKitWebView* webView, Json::Value& root) {
  Json::StreamWriterBuilder writer;
  std::string json_str = Json::writeString(writer, root);
  std::string script = "window.postMessage(" + json_str + ", '*');";
  webkit_web_view_evaluate_javascript(
    webView, script.c_str(), -1, nullptr, nullptr, nullptr, nullptr, nullptr
  );
}

void handleSuccess(
  WebKitWebView* webView,
  JSCValue* value,
  const std::function<void(Json::Value&)>& build_data
) {
  Json::Value root;
  root["type"] = "resolve";
  root["message"] = "Success";
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  root["timestamp"] = jsc_value_to_string(timestamp);
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  root["apiEnum"] = jsc_value_to_string(apiEnum);
  build_data(root["data"]);

  sendResponse(webView, root);
}

void handleError(
  WebKitWebView* webView,
  JSCValue* value,
  const std::exception& err
) {
  Json::Value root;
  root["type"] = "reject";
  root["message"] = err.what();
  JSCValue* timestamp = jsc_value_object_get_property(value, "timestamp");
  root["timestamp"] = jsc_value_to_string(timestamp);
  JSCValue* apiEnum = jsc_value_object_get_property(value, "apiEnum");
  root["apiEnum"] = jsc_value_to_string(apiEnum);
  root["data"] = Json::nullValue;

  sendResponse(webView, root);
}

void sum(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isRequestHeaderValid(value)) {
    return;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  JSCValue* args = jsc_value_object_get_property(params, "args");
  try {
    double result = 0;
    gchar** properties = jsc_value_object_enumerate_properties(args);
    for (gchar** property = properties; *property != nullptr; property++) {
      JSCValue* arg = jsc_value_object_get_property(args, *property);
      if (jsc_value_is_number(arg) != 0) {
        result += jsc_value_to_double(arg);
      } else {
        g_strfreev(properties);
        throw std::invalid_argument("Arguments must be numbers");
      }
    }
    g_strfreev(properties);
    handleSuccess(WEBKIT_WEB_VIEW(user_data), value, [&](Json::Value& root) {
      root["result"] = result;
    });
  } catch (const std::exception& e) {
    handleError(WEBKIT_WEB_VIEW(user_data), value, e);
  }
}

void log(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isRequestHeaderValid(value)) {
    return;
  }
  JSCValue* params = jsc_value_object_get_property(value, "params");
  JSCValue* message = jsc_value_object_get_property(params, "message");
  const char* message_str = jsc_value_to_string(message);
  g_print("JS Log: %s\n", message_str);  // NOLINT
  handleSuccess(WEBKIT_WEB_VIEW(user_data), value, [&](Json::Value& root) {
    root["message"] = message_str;
  });
}

void getFileList(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isRequestHeaderValid(value)) {
    return;
  }
  try {
    JSCValue* params = jsc_value_object_get_property(value, "params");
    JSCValue* path = jsc_value_object_get_property(params, "path");
    if (jsc_value_is_string(path) == 0) {
      throw std::invalid_argument("Path must be a string");
    }
    mp::GetFileListRequestPtr request =
      std::make_shared<mp::GetFileListRequest>();
    request->setPath(jsc_value_to_string(path));
    mp::GetFileListResponsePtr response = api::getFileList(request);

    handleSuccess(WEBKIT_WEB_VIEW(user_data), value, [&](Json::Value& root) {
      for (const auto& file : response->getFiles()) {
        Json::Value fileJson;
        fileJson["name"] = file.name;
        fileJson["type"] = toString(file.type);
        root["files"].append(fileJson);
      }
    });
  } catch (const std::exception& e) {
    handleError(WEBKIT_WEB_VIEW(user_data), value, e);
  }
}

void getFileDetail(
  [[maybe_unused]] WebKitUserContentManager* manager,
  WebKitJavascriptResult* js_result,
  gpointer user_data
) {
  JSCValue* value = webkit_javascript_result_get_js_value(js_result);
  if (!isRequestHeaderValid(value)) {
    return;
  }
  try {
    JSCValue* params = jsc_value_object_get_property(value, "params");
    JSCValue* path = jsc_value_object_get_property(params, "path");
    if (jsc_value_is_string(path) == 0) {
      throw std::invalid_argument("Path must be a string");
    }
    mp::GetFileDetailRequestPtr request =
      std::make_shared<mp::GetFileDetailRequest>();
    request->setPath(jsc_value_to_string(path));
    mp::GetFileDetailResponsePtr response = api::getFileDetail(request);

    handleSuccess(WEBKIT_WEB_VIEW(user_data), value, [&](Json::Value& root) {
      root = response->toJson();
    });
  } catch (const std::exception& e) {
    handleError(WEBKIT_WEB_VIEW(user_data), value, e);
  }
}
}  // namespace zipfiles::client::view