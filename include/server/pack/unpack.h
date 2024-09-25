#ifndef ZIPFILES_SERVER_UNPACK_H
#define ZIPFILES_SERVER_UNPACK_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
int unpack(char* src);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_UNPACK_H