#include <fstream>
#include <iostream>
#include <chrono>

#include "lz77.h"

using namespace zipfiles;
int main(int argc, char** argv) {
  auto stt = std::chrono::steady_clock::now();
  std::string base_dir = "/app/tests/unittest/copyfile/";
  std::string src_filename = base_dir + "example1.txt";
  std::string dst_filename = base_dir + "example1_copy.txt";
  src_filename = argv[1];
  dst_filename = argv[2];
  std::ifstream src_file(src_filename, std::ios::binary | std::ios::in);
  std::ofstream dst_file(dst_filename, std::ios::binary | std::ios::out | std::ios::trunc);
  std::string input_buffer, output_buffer;
  std::vector<uint16_t> literal_length_alphabet;
  std::vector<uint16_t> distance_alphabet;
  auto lz77 = zip::LZ77(input_buffer, literal_length_alphabet, distance_alphabet, output_buffer);
  input_buffer.resize(1<<20);
  int output_length;
  bool end = false;
  while (!end) {
    src_file.read(&input_buffer[0], 1 << 20);
    if (src_file.eof()) {
      input_buffer.resize(src_file.gcount());
      end = true;
    }
    // dst_file.write(&input_buffer[0], input_buffer.size());
    std::cout << "input length: " << lz77.encode();
    std::cout << ", output length: " << (output_length = lz77.decode()) << std::endl;
    dst_file.write(&output_buffer[0], output_length);
  }
  src_file.close();
  dst_file.close();
  std::cout << std::chrono::duration<double>(std::chrono::steady_clock::now() - stt).count() << std::endl;
  return 0;
}