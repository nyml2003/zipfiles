#ifndef ZIPFILES_UNPACK_H
#define ZIPFILES_UNPACK_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::unpack {
int unpack(char* src);
}  // namespace zipfiles::unpack
#endif  // !ZIPFILES_UNPACK_H