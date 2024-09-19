#ifndef ZIPFILES_FSAPI_H
#define ZIPFILES_FSAPI_H
#include <filesystem>
#include <vector>
#include "common.h"
namespace fs = std::filesystem;
namespace zipfiles::server {
std::vector<File> list(const fs::path& source);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_FSAPI_H