#ifndef ZIPFILES_FSAPI_H
#define ZIPFILES_FSAPI_H
#include "common.h"
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;
namespace zipfiles::server {
std::vector<File> list(const fs::path& source);
}  // namespace zipfiles::server::fsapi
#endif                        // !ZIPFILES_FSAPI_H