#ifndef ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H
#define ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H

#include <cstdint>
#include <vector>

namespace zipfiles::server {

constexpr int ZIP_BLOCK_SIZE = 1 << 20;

std::pair<bool, std::vector<uint8_t>&> zip(uint8_t byte, bool flush = false);
std::pair<bool, std::vector<uint8_t>&> unzip(uint8_t byte);

}  // namespace zipfiles::server

#endif  // ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H