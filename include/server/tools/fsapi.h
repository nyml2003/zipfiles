#ifndef ZIPFILES_SERVER_TOOLS_FSAPI_H
#define ZIPFILES_SERVER_TOOLS_FSAPI_H
#include <filesystem>
#include <vector>
#include "mp/dto.h"
namespace zipfiles::server {
std::vector<File> getFileList(const std::filesystem::path& directory);
FileDetail getFileDetail(const std::filesystem::path& file);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_FSAPI_H