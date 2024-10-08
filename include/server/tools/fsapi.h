#ifndef ZIPFILES_SERVER_TOOLS_FSAPI_H
#define ZIPFILES_SERVER_TOOLS_FSAPI_H

#include <filesystem>
#include <vector>
#include "mp/dto.h"
#include "mp/filter.h"

namespace zipfiles::server {

std::vector<File>
getFileList(const fs::path& directory, bool doFilter, MetaDataFilter& filter);
FileDetail getFileDetail(const fs::path& file);
std::vector<FileDetail> getAllFileDetails(const fs::path& directory);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_FSAPI_H