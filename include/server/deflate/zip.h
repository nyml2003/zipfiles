#ifndef ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H
#define ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H

#include <cstdint>
#include <vector>
#include "server/deflate/huffman.h"
#include "server/deflate/lz77.h"

namespace zipfiles::server {

constexpr int ZIP_BLOCK_SIZE = 1 << 20;

struct ZipStatus {
  bool flush;
  bool lack;
  std::vector<uint8_t>* obuffer;
};

class Zip {
 private:
  std::vector<uint8_t> ibuffer;         // input buffer
  std::vector<uint8_t> obuffer;         // output buffer
  std::vector<uint8_t> lc_alphabet;     // letter and length buffer
  std::vector<uint16_t> dist_alphabet;  // distance buffer
  LZ77::Encoder lz77_ecodoer;
  huffman::Encoder huffman_encoder;

 public:
  void run();
};

class Unzip {
 private:
  std::vector<uint8_t> ibuffer;         // input buffer
  std::vector<uint8_t> obuffer;         // output buffer
  std::vector<uint8_t> lc_alphabet;     // letter and length buffer
  std::vector<uint16_t> dist_alphabet;  // distance buffer
  LZ77::Decoder lz77_deodoer;
  huffman::Decoder huffman_decoder;

 public:
  void run();
};

// ! Scrapped, retain this only for compiling the backup.cpp when testing zip
std::pair<bool, std::vector<uint8_t>&> zip(uint8_t byte, bool flush = false);

ZipStatus zip(const std::vector<uint8_t>& input, bool flush = false);
std::pair<bool, std::vector<uint8_t>&> unzip(uint8_t byte);

}  // namespace zipfiles::server

#endif  // ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H