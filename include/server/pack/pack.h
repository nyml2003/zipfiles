#ifndef ZIPFILES_SERVER_PACK_H
#define ZIPFILES_SERVER_PACK_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
int pack(char* src);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_PACK_H