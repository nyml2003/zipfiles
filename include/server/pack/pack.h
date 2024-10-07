#ifndef ZIPFILES_SERVER_PACK_H
#define ZIPFILES_SERVER_PACK_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles::server {

/**
 * !Deprecated
 *
 */
std::ofstream createArchive(const std::string& archiveName);

void packFileToArchive(std::ofstream& archive, const fs::path& filePath);

/**
 * * pack
 *
 */
std::vector<uint8_t> readChunkFromFile(std::ifstream& inFile, size_t chunkSize);

size_t insertHeader(
  const fs::path& filePath,
  size_t dataSize,
  std::vector<uint8_t>& target,
  size_t targetOffset
);

std::pair<bool, std::vector<uint8_t>&>
packFilesByBlock(const std::vector<fs::path>& files, bool flush);

}  // namespace zipfiles::server

#endif  // !ZIPFILES_PACK_H