#include "lz77.h"
#include "utils.h"

constexpr int INPUTBUF_SIZE = 1 << 20;

using zipfiles::close_ifile;
using zipfiles::close_ofile;
using zipfiles::read_buf;
using zipfiles::set_ifile;
using zipfiles::set_ofile;
using zipfiles::write_buf;
using zipfiles::zip::LZ77;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return 1;
  }
  std::string src_filename = argv[1];
  std::string dst_filename = argv[2];
  set_ifile(src_filename);
  set_ofile(dst_filename);
  std::vector<uint8_t> input_buffer;
  std::vector<uint8_t> output_buffer;
  std::vector<uint8_t> literal_length_alphabet;
  std::vector<uint16_t> distance_alphabet;
  auto lz77 = LZ77(literal_length_alphabet, distance_alphabet);
  int len = 0;
  input_buffer.resize(INPUTBUF_SIZE);
  while ((len = read_buf(input_buffer.data(), INPUTBUF_SIZE))) {
    input_buffer.resize(len);
    lz77.encode(input_buffer);
    lz77.decode(output_buffer);
    write_buf(output_buffer.data(), output_buffer.size());
  }
  close_ifile();
  close_ofile();
  return 0;
}