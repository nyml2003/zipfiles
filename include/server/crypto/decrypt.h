#ifndef ZIPFILES_DECRYPT_H
#define ZIPFILES_DECRYPT_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::decrypt {
char* decrypt(char* src);
}  // namespace zipfiles::decrypt
#endif  // !ZIPFILES_DECRYPT_H