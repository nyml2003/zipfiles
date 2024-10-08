#include <gtest/gtest.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <vector>

#include "server/deflate/zip.h"

bool test(const std::string& src_filename);

TEST(ZIP, test1) {
  ASSERT_TRUE(test("test.txt"));
}

TEST(ZIP, test2) {
  ASSERT_TRUE(test("test.pdf"));
}

TEST(ZIP, test3) {
  ASSERT_TRUE(test("test.exe"));
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
  const int ibuf_size = (1143 << 16) + 191981;
  std::vector<std::uint8_t> ibuffer(ibuf_size);
  while (!ifile.eof()) {
    ifile.read(reinterpret_cast<char*>(ibuffer.data()), ibuf_size);
    ibuffer.resize(ifile.gcount());
    ZipStatus zip_ret = {false, false, nullptr};
    while (!zip_ret.lack) {
      zip_ret = zip(ibuffer, ifile.eof());
      if (zip_ret.flush) {
        ofile.write(
          reinterpret_cast<char*>(zip_ret.obuffer->data()),
          static_cast<int>(zip_ret.obuffer->size())
        );
      }
    }
  }
  ifile.close();
  ofile.close();

  // unzip
  ifile.open(base_dir + dst_filename, iflag);
  ofile.open(base_dir + src_filename_ex, oflag);
  char buf = 0;
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