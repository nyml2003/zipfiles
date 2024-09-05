#include <gtest/gtest.h>
#include <fstream>
#include "copyfile.h"

using namespace zipfiles;
TEST(CopyFile, copyfile) {
  std::string base_dir = "/app/tests/unittest/copyfile/";
  std::string src_filename = base_dir + "example1.txt";
  std::string dst_filename = base_dir + "example1_copy.txt";

  copyfile(src_filename, dst_filename);

  std::ifstream src_file(src_filename, std::ios::binary);
  std::ifstream dst_file(dst_filename, std::ios::binary);
  char src_buffer, dst_buffer;

  while (true) {
    src_file.read(&src_buffer, sizeof(src_buffer));
    dst_file.read(&dst_buffer, sizeof(dst_buffer));
    if (src_file.eof() || dst_file.eof())
      break;
    EXPECT_EQ(src_buffer, dst_buffer);
  }
  src_file.close();
  dst_file.close();
  remove(dst_filename.c_str());
}