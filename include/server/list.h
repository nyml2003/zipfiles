#ifndef ZIPFILES_SERVER_LIST_H
#define ZIPFILES_SERVER_LIST_H
#include "mp/GetFileList.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
namespace zipfiles::server {
std::vector<mp::File> list(const fs::path& source);
}
#endif  // !ZIPFILES_SERVER_LIST_H