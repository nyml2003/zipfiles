#ifndef ZIPFILES_SERVER_UNPACK_H
#define ZIPFILES_SERVER_UNPACK_H

#include <cstdint>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles::server {

constexpr int UNPACK_BLOCK_SIZE = 1 << 21;

enum class State {
  READ_PATH_SIZE,
  READ_PATH,
  READ_DATA_SIZE,
  READ_DATA,
  FLUSH
};

/**
 * ! Deprecated
 *
 */
void unpackArchive(const fs::path& archivePath, const fs::path& outputDir);

void unpackFiles(const std::vector<uint8_t>& packedData, const fs::path& dst);

/**
 * * unpack
 *
 */
std::pair<bool, std::vector<uint8_t>&> unpackFilesByBlock(
  std::vector<uint8_t>& ibuffer,
  const fs::path& dst,
  bool flush
);

}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_UNPACK_H