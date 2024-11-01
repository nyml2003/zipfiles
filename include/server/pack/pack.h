#ifndef ZIPFILES_SERVER_PACK_H
#define ZIPFILES_SERVER_PACK_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>
#include "server/tools/fsapi.h"

namespace zipfiles::server {
// PACK块大小
constexpr int PACK_BLOCK_SIZE = 1 << 20;

/**
 * * pack
 *
 */
void fileDetailSerialize(
  const FileDetail& fileDetail,
  std::vector<uint8_t>& header,
  size_t structSize
);

void createHeader(
  const fs::path& filePath,
  const FileDetail& fileDetail,
  std::vector<uint8_t>& header
);

std::pair<bool, std::vector<uint8_t>&> packFilesByBlock(
  const std::vector<fs::path>& files,
  bool flush,
  const fs::path& lca
);

}  // namespace zipfiles::server

#endif  // !ZIPFILES_PACK_H