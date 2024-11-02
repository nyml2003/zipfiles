#ifndef ZIPFILES_SERVER_CONFIGURE_H
#define ZIPFILES_SERVER_CONFIGURE_H

#include <filesystem>

namespace fs = std::filesystem;

namespace zipfiles::server {
// log文件地址
const fs::path COMMIT_TABLE_PATH = "/usr/local/zipfiles/.zip/commit_table";

// 存储路径
const fs::path STORAGE_PATH = "/usr/local/zipfiles/.zip";

}  // namespace zipfiles::server

#endif  // ZIPFILES_SERVER_CONFIGURE_H