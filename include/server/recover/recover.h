#ifndef ZIPFILES_RECOVER_H
#define ZIPFILES_RECOVER_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
int recover_to(const fs::path& dst);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_RECOVER_H