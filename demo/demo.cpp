#include <fstream>
#include <iostream>

#include "lz77.h"
#include "utils.h"

constexpr int INPUTBUF_SIZE = 1 << 20;

using namespace zipfiles;
int main(int argc, char** argv) {
  std::string src_filename = argv[1];
  std::string dst_filename = argv[2];
  set_ifile(src_filename);
  set_ofile(dst_filename);
  std::vector<uint8_t> input_buffer, output_buffer;
  std::vector<uint8_t> literal_length_alphabet;
  std::vector<uint16_t> distance_alphabet;
  auto lz77 = zip::LZ77(
    input_buffer, literal_length_alphabet, distance_alphabet, output_buffer
  );
  int output_length;
  int len;
  input_buffer.resize(INPUTBUF_SIZE);
  while (len = read_buf(&input_buffer[0], INPUTBUF_SIZE)) {
    input_buffer.resize(len);
    lz77.encode();
    output_length = lz77.decode();
    write_buf(&output_buffer[0], output_length);
  }
  close_ifile();
  close_ofile();
  return 0;
}