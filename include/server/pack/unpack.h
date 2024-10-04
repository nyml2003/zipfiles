#ifndef ZIPFILES_SERVER_UNPACK_H
#define ZIPFILES_SERVER_UNPACK_H

#include <filesystem>
#include <vector>
#include "server/crypto/crypto.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

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

void unpackAndWriteBackFilesByBlock(
  const fs::path& dst,
  std::array<CryptoPP::byte, AES::BLOCKSIZE>& iv
);

}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_UNPACK_H