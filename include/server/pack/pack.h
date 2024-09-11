#ifndef ZIPFILES_PACK_H
#define ZIPFILES_PACK_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::pack {
int pack(char* src);
}  // namespace zipfiles::pack
#endif  // !ZIPFILES_PACK_H