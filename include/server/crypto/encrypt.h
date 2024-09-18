#ifndef ZIPFILES_ENCRYPT_H
#define ZIPFILES_ENCRYPT_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
char* encrypt(char* src);
}  // namespace zipfiles::server::encrypt
#endif  // !ZIPFILES_ENCRYPT_H