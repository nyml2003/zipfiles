#ifndef ZIPFILES_ENCRYPT_H
#define ZIPFILES_ENCRYPT_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::encrypt {
char* encrypt(char* src);
}  // namespace zipfiles::encrypt
#endif  // !ZIPFILES_ENCRYPT_H