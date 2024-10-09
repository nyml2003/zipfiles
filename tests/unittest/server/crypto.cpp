#include <gtest/gtest.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <vector>

#include "server/crypto/crypto.h"

bool test(const std::string& src_filename);

TEST(CRYPTO, test1) {
  ASSERT_TRUE(test("test.txt"));
}

TEST(CRYPTO, test2) {
  ASSERT_TRUE(test("test.pdf"));
}

TEST(CRYPTO, test3) {
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
  std::ofstream log;
  auto iflag = std::ios::in | std::ios::binary;
  auto oflag = std::ios::out | std::ios::binary | std::ios::trunc;
  ifile.open(base_dir + src_filename, iflag);
  ofile.open(base_dir + dst_filename, oflag);
  log.open(base_dir + "log", oflag);
  const int ibuf_size = (1143 << 16) + 191981;
  std::vector<std::uint8_t> ibuffer(ibuf_size);
  AESEncryptor encryptor("123456");
  std::array<CryptoPP::byte, AES::BLOCKSIZE> iv{};

  AutoSeededRandomPool prng;
  prng.GenerateBlock(iv.data(), iv.size());
  ofile.write(reinterpret_cast<const char*>(iv.data()), iv.size());
  while (!ifile.eof()) {
    ifile.read(reinterpret_cast<char*>(ibuffer.data()), ibuf_size);
    ibuffer.resize(ifile.gcount());
    CryptStatus crypt_ret = {false, false, nullptr};
    while (!crypt_ret.lack) {
      crypt_ret = encryptor.encryptFile(ibuffer, iv, ifile.eof());
      if (crypt_ret.flush) {
        ofile.write(
          reinterpret_cast<char*>(crypt_ret.obuffer->data()),
          static_cast<int>(crypt_ret.obuffer->size())
        );
      }
    }
  }
  ifile.close();
  ofile.close();

  // unzip
  ifile.open(base_dir + dst_filename, iflag);
  ofile.open(base_dir + src_filename_ex, oflag);
  ifile.read(reinterpret_cast<char*>(iv.data()), iv.size());
  ibuffer.resize(ibuf_size);
  while (!ifile.eof()) {
    ifile.read(reinterpret_cast<char*>(ibuffer.data()), ibuf_size);
    ibuffer.resize(ifile.gcount());
    CryptStatus crypt_ret = {false, false, nullptr};
    while (!crypt_ret.lack) {
      crypt_ret = encryptor.decryptFile(ibuffer, iv, ifile.eof());
      if (crypt_ret.flush) {
        ofile.write(
          reinterpret_cast<char*>(crypt_ret.obuffer->data()),
          static_cast<int>(crypt_ret.obuffer->size())
        );
      }
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