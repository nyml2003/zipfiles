// #ifndef ZIPFILES_SERVER_CONFIGURE_H
// #define ZIPFILES_SERVER_CONFIGURE_H

// #include <filesystem>
// #include <string>

// namespace fs = std::filesystem;
// namespace zipfiles::server {
// struct Configure {
//   size_t port;
//   std::string backup_path;
// };
// class ConfigureHandler {
//  public:
//   static ConfigureHandler& get_instance();

//   // 禁止拷贝和赋值
//   ConfigureHandler(const ConfigureHandler&) = delete;
//   ConfigureHandler& operator=(const ConfigureHandler&) = delete;

//   // 从文件加载配置
//   static void load_configure();

//   // 获取配置
//   [[nodiscard]] Configure get_configure() const;

//   // 更新配置
//   static void update_configure(const Configure& new_config);

//  private:
//   static constexpr const char* configure_path = "/etc/zipfiles/config.json";
//   ConfigureHandler();
//   ~ConfigureHandler();

//   static void ensure_config_file_exists();
//   void save_configure() const;

//   Configure configure_;
// };

// }  // namespace zipfiles::server

// #endif  // ZIPFILES_SERVER_CONFIGURE_H