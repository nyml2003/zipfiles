#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <ios>

#include "server/deflate/zip.h"

bool test(const std::string& src_filename);

TEST(ZIP, test1) {
  std::string src_filename = "test.txt";
  ASSERT_TRUE(test(src_filename));
}

TEST(ZIP, test2) {
  std::string src_filename = "test.pdf";
  ASSERT_TRUE(test(src_filename));
}

TEST(ZIP, test3) {
  std::string src_filename = "test.exe";
  ASSERT_TRUE(test(src_filename));
}

bool test(const std::string& src_filename) {
  using namespace zipfiles::server;
  std::string base_dir = "/app/tests/unittest/data/";
  std::string dst_filename = src_filename + ".zipfile";
  std::string src_filename_ex = src_filename + ".copy";

  // zip
  std::ifstream ifile;
  std::ofstream ofile;
  auto iflag = std::ios::in | std::ios::binary;
  auto oflag = std::ios::out | std::ios::binary | std::ios::trunc;
  ifile.open(base_dir + src_filename, iflag);
  ofile.open(base_dir + dst_filename, oflag);
  char buf = 0;
  // int64_t count = 0;
  ifile.get(buf);
  while (!ifile.eof()) {
    auto [flush, obuffer] = zip(buf, (ifile.get(buf), ifile.eof()));
    if (flush) {
      ofile.write(
        reinterpret_cast<char*>(obuffer.data()),
        static_cast<int>(obuffer.size())
      );
    }
  }
  ifile.close();
  ofile.close();

  // unzip
  ifile.open(base_dir + dst_filename, iflag);
  ofile.open(base_dir + src_filename_ex, oflag);
  while (ifile.get(buf), !ifile.eof()) {
    auto [flush, obuffer] = unzip(buf);
    if (flush) {
      ofile.write(
        reinterpret_cast<char*>(obuffer.data()),
        static_cast<int>(obuffer.size())
      );
    }
  }
  ifile.close();
  ofile.close();

  // compare
  std::ifstream src1(base_dir + src_filename, iflag);
  std::ifstream src2(base_dir + src_filename_ex, iflag);
  char b1 = 0;
  char b2 = 0;
  while (true) {
    src1.get(b1);
    src2.get(b2);
    if (src1.eof() != src2.eof()) {
      return false;
    }
    if (src1.eof() && src2.eof()) {
      return true;
    }
    if (b1 != b2) {
      return false;
    }
  }
  src1.close();
  src2.close();
}