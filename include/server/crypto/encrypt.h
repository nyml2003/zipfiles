#ifndef ZIPFILES_SERVER_ENCRYPT_H
#define ZIPFILES_SERVER_ENCRYPT_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
char* encrypt(char* src);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_ENCRYPT_H