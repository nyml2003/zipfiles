#ifndef ZIPFILES_DECRYPT_H
#define ZIPFILES_DECRYPT_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
char* decrypt(char* src);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_DECRYPT_H