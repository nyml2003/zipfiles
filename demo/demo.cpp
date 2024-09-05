#include <iostream>

#include "copyfile.h"

using namespace zipfiles;
int main(int argc, char** argv) {
  std::string base_dir = "/app/tests/unittest/copyfile/";
  std::string src_filename = base_dir + "example1.txt";
  std::string dst_filename = base_dir + "example1_copy.txt";

  copyfile(src_filename, dst_filename);
  return 0;
}