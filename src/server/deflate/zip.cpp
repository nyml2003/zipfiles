#include "server/deflate/zip.h"
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
    ibuffer.reserve(ZIP_BLOCK_SIZE + 3);
    obuffer.reserve(ZIP_BLOCK_SIZE + 3);
    lc_alphabet.reserve(ZIP_BLOCK_SIZE + 3);
    dist_alphabet.reserve(ZIP_BLOCK_SIZE + 3);
  }
};

std::pair<bool, std::vector<uint8_t>&> zip(uint8_t byte, bool flush) {
  static thread_local ZipBuffer buffer;
  static thread_local LZ77::LZ77 lz77(buffer.lc_alphabet, buffer.dist_alphabet);
  static thread_local huffman::Encoder encoder(
    buffer.lc_alphabet, buffer.dist_alphabet, buffer.obuffer
  );

  buffer.ibuffer.push_back(byte);

  // if data is not enough or not flush, wait for more data
  if (buffer.ibuffer.size() < ZIP_BLOCK_SIZE && !flush) {
    return {false, buffer.obuffer};
  }

  // zip
  lz77.encode(buffer.ibuffer);
  encoder.encode();
  buffer.ibuffer.clear();

  return {true, buffer.obuffer};
  // TODO(zxn_64)
}

std::pair<bool, std::vector<uint8_t>&> unzip(uint8_t byte) {
  static thread_local ZipBuffer buffer;
  static thread_local LZ77::LZ77 lz77(buffer.lc_alphabet, buffer.dist_alphabet);
  static thread_local huffman::Decoder decoder(
    buffer.lc_alphabet, buffer.dist_alphabet
  );

  if (!decoder.decode(byte)) {
    return {false, buffer.obuffer};
  }
  lz77.decode(buffer.obuffer);

  return {true, buffer.obuffer};
  // TODO(zxn_64)
}
}  // namespace zipfiles::server