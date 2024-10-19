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
void restoreTo(
  const fs::path& dst,
  const std::string& uuid,
  const std::string& key
);

/**
 * * directory
 *
 */
Json::Value readDirectoryFileById(const std::string& uuid);

Json::Value readDirectoryFile(const fs::path& src);

}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_RESTORE_H