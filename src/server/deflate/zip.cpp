#include <cassert>
#include <cstdint>
#include <vector>

#include "server/deflate/huffman.h"
#include "server/deflate/lz77.h"
#include "server/deflate/zip.h"

namespace zipfiles::server {

void Zip::reset_input(const std::vector<uint8_t>* input_ptr, bool eof) {
  input = input_ptr;
  flush = eof;
  ibuf_start = input->begin();
}

ZipStatus Zip::run() {
  auto append_ibuffer = [&](int len) {
    buffer.ibuffer.insert(buffer.ibuffer.end(), ibuf_start, ibuf_start + len);
    ibuf_start += len;
    assert(ibuf_start <= input->end());
  };

  bool lack = (ibuf_start == input->end());

  if (!lack) {
    append_ibuffer(std::min(
      static_cast<int>(ZIP_BLOCK_SIZE - buffer.ibuffer.size()),
      static_cast<int>(input->end() - ibuf_start)
    ));
    lack = (ibuf_start == input->end());

    // if data is enough or force flush, zip and flush
    if (buffer.ibuffer.size() == ZIP_BLOCK_SIZE || flush) {
      // zip
      lz77_ecodoer.run();
      huffman_encoder.encode();
      buffer.ibuffer.clear();
      return {true, lack, &buffer.obuffer};
    }
  }
  return {false, true, nullptr};
}

void Unzip::reset_input(const std::vector<uint8_t>* input_ptr) {
  input = input_ptr;
  ibuf_start = input->begin();
}

ZipStatus Unzip::run() {
  auto reset_ibuffer = [&]() {
    buffer.ibuffer.clear();
    int len = std::min(
      static_cast<int>(ZIP_BLOCK_SIZE),
      static_cast<int>(input->end() - ibuf_start)
    );
    buffer.ibuffer.insert(buffer.ibuffer.end(), ibuf_start, ibuf_start + len);
    ibuf_start += len;
    huffman_decoder.reset_ibuf();
    assert(ibuf_start <= input->end());
  };

  if (ibuf_start == input->begin()) {
    reset_ibuffer();
  }

  while (!huffman_decoder.decode()) {
    if (ibuf_start == input->end()) {
      return {false, true, nullptr};
    }
    reset_ibuffer();
  }
  lz77_decoder.run();
  return {true, false, &buffer.obuffer};
}

ZipStatus unzip(const std::vector<uint8_t>& input) {
  static std::vector<uint8_t> buffer(input);
  return {false, true, &buffer};
}

ZipStatus zip(const std::vector<uint8_t>& input, bool flush) {
  static std::vector<uint8_t> buffer(input);
  return {flush, true, &buffer};
}

std::pair<bool, std::vector<uint8_t>&> unzip(uint8_t byte) {
  static std::vector<uint8_t> ret{byte};
  return {false, ret};
}

}  // namespace zipfiles::server