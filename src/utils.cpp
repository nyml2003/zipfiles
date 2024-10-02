#include "utils.h"

#include <iostream>
#include <fstream>

namespace zipfiles {

uint64_t read_bytes = 0;  // NOLINT
std::ifstream ifile;      // NOLINT
std::ofstream ofile;      // NOLINT

int add(int a, int b) {
  return a + b;
}

void set_ifile(const std::string& filename) {
  if (ifile.is_open()) {
    ifile.close();
  }
  ifile.open(filename, std::ios::binary | std::ios::in);
  if (ifile.fail()) {
    std::cerr << "Open file: " << filename << "Failed" << std::endl;
    exit(1);
  }
}

void set_ofile(const std::string& filename) {
  if (ofile.is_open()) {
    ofile.close();
  }
  ofile.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
  if (ofile.fail()) {
    std::cerr << "Open file: " << filename << "Failed" << std::endl;
    exit(1);
  }
}

void close_ifile() {
  ifile.close();
}

void close_ofile() {
  ofile.close();
}

int read_buf(uint8_t* buf, uint32_t size) {
  if (ifile.eof()) {
    return 0;
  }
  ifile.read(reinterpret_cast<char*>(buf), size);
  int len = static_cast<int>(ifile.gcount());
  read_bytes += len;
  return len;
}

void write_buf(uint8_t* buf, uint32_t size) {
  ofile.write(reinterpret_cast<char*>(buf), size);
}

}  // namespace zipfiles