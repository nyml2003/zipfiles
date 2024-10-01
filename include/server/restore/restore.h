#ifndef ZIPFILES_SERVER_RESTORE_H
#define ZIPFILES_SERVER_RESTORE_H

#include <filesystem>
#include "json/value.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

/**
 * * restore
 *
 */
void restoreTo(const fs::path& dst, const std::string& uuid);

/**
 * * commit log
 *
 */
Json::Value readCommitLog(const fs::path& src);

Json::Value getCommitLogById(const Json::Value& cls, const std::string& uuid);

/**
 * * directory
 *
 */
Json::Value readDirectoryFile(const fs::path& src);

}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_RESTORE_H