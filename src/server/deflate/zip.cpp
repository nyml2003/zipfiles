#include <cstdint>
#include <vector>

#include "server/deflate/huffman.h"
#include "server/deflate/lz77.h"
#include "server/deflate/zip.h"

namespace zipfiles::server {

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

ZipStatus zip(const std::vector<uint8_t>& input, bool flush) {
  static thread_local ZipBuffer buffer;
  static thread_local LZ77::Encoder lz77(
    buffer.ibuffer, buffer.lc_alphabet, buffer.dist_alphabet
  );
  static thread_local huffman::Encoder encoder(
    buffer.lc_alphabet, buffer.dist_alphabet, buffer.obuffer
  );

  static thread_local size_t last = 0;  // the start index of last input
  auto append_ibuffer = [&](size_t len) {
    buffer.ibuffer.insert(
      buffer.ibuffer.end(),
      input.begin() + static_cast<std::vector<uint8_t>::difference_type>(last),
      input.begin() +
        static_cast<std::vector<uint8_t>::difference_type>(last + len)
    );
    last += len;
  };

  bool lack = (last == input.size());

  if (!lack) {
    append_ibuffer(
      std::min(ZIP_BLOCK_SIZE - buffer.ibuffer.size(), input.size() - last)
    );
    lack = (last == input.size());
    last %= input.size();

    // if data is enough or force flush, zip and flush
    if (buffer.ibuffer.size() == ZIP_BLOCK_SIZE || flush) {
      // zip
      lz77.run();
      encoder.encode();
      buffer.ibuffer.clear();
      return {true, lack, &buffer.obuffer};
    }
  }
  last = 0;
  return {false, true, &buffer.obuffer};
}

std::pair<bool, std::vector<uint8_t>&> unzip(uint8_t byte) {
  static thread_local ZipBuffer buffer;
  static thread_local LZ77::Decoder lz77(
    buffer.lc_alphabet, buffer.dist_alphabet, buffer.obuffer
  );
  static thread_local huffman::Decoder decoder(
    buffer.lc_alphabet, buffer.dist_alphabet
  );

  if (!decoder.decode(byte)) {
    return {false, buffer.obuffer};
  }
  lz77.run();

  return {true, buffer.obuffer};
}
}  // namespace zipfiles::server