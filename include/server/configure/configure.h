#ifndef ZIPFILES_CONFIGURE_H
#define ZIPFILES_CONFIGURE_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
class ConfigureHandler {
   public:
    static ConfigureHandler& get_instance() {
        static ConfigureHandler instance;
        return instance;
    }

    // forbid copy and assignment
    ConfigureHandler(const ConfigureHandler&) = delete;
    ConfigureHandler& operator=(const ConfigureHandler&) = delete;

    // save configure to disk
    static void save_configure(const fs::path& dst);

    // read configure from disk
    static void read_configure(const fs::path& src);

   private:
    ConfigureHandler();
    ~ConfigureHandler();
};
}  // namespace zipfiles::server
#endif  // !ZIPFILES_CONFIGURE_H