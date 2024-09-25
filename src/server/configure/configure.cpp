// #include "server/configure/configure.h"
// #include <fstream>
// #include <json/json.h>  // 使用 JSON 库来解析和创建配置文件

// namespace zipfiles::server {

// ConfigureHandler& ConfigureHandler::get_instance() {
//   static ConfigureHandler instance;
//   return instance;
// }

// ConfigureHandler::ConfigureHandler() {
//   ensure_config_file_exists();
//   load_configure();
// }

// ConfigureHandler::~ConfigureHandler() = default;

// void ConfigureHandler::ensure_config_file_exists() {
//   if (!fs::exists(configure_path)) {
//     Json::Value default_config;
//     default_config["port"] = 8080;
//     default_config["backup_path"] = "/default/path";
//     std::ofstream file(configure_path);
//     file << default_config.toStyledString();  // 格式化输出 JSON
//   }
// }

// void ConfigureHandler::load_configure() {
//   std::ifstream file(configure_path, std::ifstream::binary);
//   if (file.is_open()) {
//     Json::Value root;
//     file >> root;
//     configure_.port = root.get("port", configure_.port).asUInt();
//     configure_.backup_path =
//       root.get("backup_path", configure_.backup_path).asString();
//   }
// }

// Configure ConfigureHandler::get_configure() const {
//   return configure_;
// }

// void ConfigureHandler::update_configure(const Configure& new_config) {
//   configure_ = new_config;
//   save_configure();
// }

// void ConfigureHandler::save_configure() const {
//   Json::Value root;
//   root["port"] = configure_.port;
//   root["backup_path"] = configure_.backup_path;
//   std::ofstream file(configure_path);
//   file << root.toStyledString();  // 格式化输出 JSON
// }

// }  // namespace zipfiles::server