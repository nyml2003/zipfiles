#ifndef ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H
#define ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H

#include <cstdint>
#include <vector>
#include "server/deflate/huffman.h"
#include "server/deflate/lz77.h"

namespace zipfiles::server {

constexpr int ZIP_BLOCK_SIZE = 1 << 20;

struct ZipBuffer {
  std::vector<uint8_t> ibuffer;         // input buffer
  std::vector<uint8_t> obuffer;         // output buffer
  std::vector<uint8_t> lc_alphabet;     // letter and length buffer
  std::vector<uint16_t> dist_alphabet;  // distance buffer
  ZipBuffer() {
    ibuffer.reserve(ZIP_BLOCK_SIZE + 5);
    obuffer.reserve(ZIP_BLOCK_SIZE + 5);
    lc_alphabet.reserve(ZIP_BLOCK_SIZE + 5);
    dist_alphabet.reserve(ZIP_BLOCK_SIZE + 5);
  }
};

struct ZipStatus {
  bool flush;
  bool lack;
  std::vector<uint8_t>* obuffer;
};

class Zip {
 private:
  const std::vector<uint8_t>* input = nullptr;
  std::vector<uint8_t>::const_iterator ibuf_start;
  bool flush = false;

  ZipBuffer buffer;
  LZ77::Encoder lz77_ecodoer;
  huffman::Encoder huffman_encoder;

 public:
  Zip()
    : lz77_ecodoer(buffer.ibuffer, buffer.lc_alphabet, buffer.dist_alphabet),
      huffman_encoder(
        buffer.lc_alphabet,
        buffer.dist_alphabet,
        buffer.obuffer
      ) {}
  void reset_input(const std::vector<uint8_t>* input_ptr, bool eof = false);
  ZipStatus run();
};

class Unzip {
 private:
  const std::vector<uint8_t>* input = nullptr;
  std::vector<uint8_t>::const_iterator ibuf_start;

  ZipBuffer buffer;
  LZ77::Decoder lz77_decoder;
  huffman::Decoder huffman_decoder;

 public:
  Unzip()
    : lz77_decoder(buffer.lc_alphabet, buffer.dist_alphabet, buffer.obuffer),
      huffman_decoder(
        buffer.ibuffer,
        buffer.lc_alphabet,
        buffer.dist_alphabet
      ) {}
  void reset_input(const std::vector<uint8_t>* input_ptr);
  ZipStatus run();
};

// ! deprecated
// [[deprecated]] ZipStatus
// zip(const std::vector<uint8_t>& input, bool flush = false);
// [[deprecated]] ZipStatus unzip(const std::vector<uint8_t>& input);
// [[deprecated]] std::pair<bool, std::vector<uint8_t>&> unzip(uint8_t byte);

}  // namespace zipfiles::server

#endif  // ZIPFILES_INCLUDE_SERVER_DEFLATE_ZIP_H