#ifndef ZIPFILES_SERVER_TOOLS_FSAPI_H
#define ZIPFILES_SERVER_TOOLS_FSAPI_H
#include <filesystem>
#include <vector>
#include "mp/dto.h"
namespace zipfiles::server {
namespace fs = std::filesystem;
std::vector<File> getFileList(const fs::path& directory);
FileDetail getFileDetail(const fs::path& file);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_FSAPI_H