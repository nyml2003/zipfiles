#ifndef ZIPFILES_PACK_H
#define ZIPFILES_PACK_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
int pack(char* src);
}  // namespace zipfiles::server::pack
#endif  // !ZIPFILES_PACK_H