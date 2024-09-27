#include "client/view.h"
#include <iostream>
#include <memory>
#include <thread>
#include "client/api.h"
#include "client/launcher.h"
#include "jsc/jsc.h"
#include "json/value.h"
#include "utils.h"
#include <future>
namespace zipfiles::client::view {
bool isRequestHeaderValid(JSCValue* value) {
  if (value == nullptr) {
    g_print("js_add_function: value is null\n");  // NOLINT
    return false;
  }
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

void sendResponse(Json::Value& root) {
  if (launcher::webView == nullptr) {
    return;
  }
  Json::StreamWriterBuilder writer;
  std::string json_str = Json::writeString(writer, root);
  std::string script = "window.postMessage(" + json_str + ", '*');";
  std::cout << "evaluating script: " << script << std::endl;
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
  build_data(root["data"]);

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

  sendResponse(root);
}

// void sum(
//   [[maybe_unused]] WebKitUserContentManager* manager,
//   WebKitJavascriptResult* js_result,
//   [[maybe_unused]] gpointer user_data
// ) {
//   JSCValue* value = webkit_javascript_result_get_js_value(js_result);
//   if (!isRequestHeaderValid(value)) {
//     return;
//   }
//   JSCValue* params = jsc_value_object_get_property(value, "params");
//   JSCValue* args = jsc_value_object_get_property(params, "args");
//   try {
//     double result = 0;
//     gchar** properties = jsc_value_object_enumerate_properties(args);
//     for (gchar** property = properties; *property != nullptr; property++) {
//       JSCValue* arg = jsc_value_object_get_property(args, *property);
//       if (jsc_value_is_number(arg) != 0) {
//         result += jsc_value_to_double(arg);
//       } else {
//         g_strfreev(properties);
//         throw std::invalid_argument("Arguments must be numbers");
//       }
//     }
//     g_strfreev(properties);
//     handleSuccess(value, [&](Json::Value& root) { root["result"] = result;
//     });
//   } catch (const std::exception& e) {
//     handleFatal(e);
//   }
// }

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
  g_print("JS Log: %s\n", message_str);  // NOLINT
  handleSuccess(root, [&](Json::Value& root) {
    root["message"] = message_str;
  });
}

void getFileList(
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
    JSCValue* path = jsc_value_object_get_property(params, "path");
    if (jsc_value_is_string(path) == 0) {
      throw std::invalid_argument("Path must be a string");
    }
    mp::GetFileListRequestPtr request =
      std::make_shared<mp::GetFileListRequest>();
    request->setPath(jsc_value_to_string(path));

    Json::Value root;
    root["timestamp"] =
      jsc_value_to_double(jsc_value_object_get_property(value, "timestamp"));
    root["apiEnum"] =
      jsc_value_to_string(jsc_value_object_get_property(value, "apiEnum"));
    // 异步调用 api::getFileList
    auto future =
      std::async(std::launch::async, api::getFileListAsync, request);

    // 在后台线程中处理结果
    std::thread([root, future = std::move(future)]() mutable {
      try {
        auto response = future.get();
        handleSuccess(root, [&](Json::Value& root) {
          for (const auto& file : response.get()->getFiles()) {
            Json::Value fileJson;
            fileJson["name"] = file.name;
            fileJson["type"] = toString(file.type);
            root["files"].append(fileJson);
          }
        });
      } catch (const std::exception& e) {
        handleFatal(e);
      }
    }).detach();
  } catch (const std::exception& e) {
    handleFatal(e);
  }
}

void getFileDetail(
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
    JSCValue* path = jsc_value_object_get_property(params, "path");
    if (jsc_value_is_string(path) == 0) {
      throw std::invalid_argument("Path must be a string");
    }
    mp::GetFileDetailRequestPtr request =
      std::make_shared<mp::GetFileDetailRequest>();
    request->setPath(jsc_value_to_string(path));

    Json::Value root;
    root["timestamp"] =
      jsc_value_to_double(jsc_value_object_get_property(value, "timestamp"));
    root["apiEnum"] =
      jsc_value_to_string(jsc_value_object_get_property(value, "apiEnum"));
    // 异步调用 api::getFileDetail
    auto future =
      std::async(std::launch::async, api::getFileDetailAsync, request);

    // 在后台线程中处理结果
    std::thread([root, future = std::move(future)]() mutable {
      try {
        auto response = future.get();
        handleSuccess(root, [&](Json::Value& root) {
          root = response.get()->toJson();
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