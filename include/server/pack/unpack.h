#ifndef ZIPFILES_SERVER_UNPACK_H
#define ZIPFILES_SERVER_UNPACK_H
#include <string>

namespace zipfiles::server {

void unpackArchive(
  const std::string& archivePath,
  const std::string& outputDir
);

}  // namespace zipfiles::server
#endif  // !ZIPFILES_UNPACK_H