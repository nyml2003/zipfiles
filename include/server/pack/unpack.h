#ifndef ZIPFILES_SERVER_UNPACK_H
#define ZIPFILES_SERVER_UNPACK_H

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles::server {

/**
 * ! Deprecated
 *
 */
void unpackArchive(const fs::path& archivePath, const fs::path& outputDir);

void unpackFiles(const std::vector<uint8_t>& packedData, const fs::path& dst);

}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_UNPACK_H