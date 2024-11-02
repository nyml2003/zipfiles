#include <gtest/gtest.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

#include "server/crypto/crypto.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

std::string crypt_path = "/tmp/crypt/";       // NOLINT
std::string test_files = "/app/test_files/";  // NOLINT

// 确保包含CryptoPP库和你的Cryptor类定义

bool test(const std::string& src_filename) {
  std::string dst_filename = src_filename + ".encrypt";
  std::string src_filename_ex = src_filename + ".decrypt";

  // encrypt
  std::ifstream ifile;
  std::ofstream ofile;
  auto iflag = std::ios::in | std::ios::binary;
  auto oflag = std::ios::out | std::ios::binary | std::ios::trunc;

  fs::path ipath(test_files + src_filename);
  fs::path opath(crypt_path + dst_filename);
  fs::create_directories(opath.parent_path());

  ifile.open(ipath, iflag);
  ofile.open(opath, oflag);

  if (!ifile) {
    std::cout << "Can't open file: " << ipath << std::endl;
    return false;
  }
  if (!ofile) {
    std::cout << "Can't open file: " << opath << std::endl;
    return false;
  }

  const int ibuf_size = (1143 << 16) + 191981;
  std::vector<std::uint8_t> ibuffer(ibuf_size);
  Cryptor encryptor("123456");
  std::array<CryptoPP::byte, AES::BLOCKSIZE> iv{};

  AutoSeededRandomPool prng;
  prng.GenerateBlock(iv.data(), iv.size());
  ofile.write(reinterpret_cast<const char*>(iv.data()), iv.size());

  // Start timing for encryption
  auto start_encrypt = std::chrono::high_resolution_clock::now();

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

  // End timing for encryption and output the time taken
  auto end_encrypt = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> encryption_time =
    end_encrypt - start_encrypt;
  std::cout << "Encryption time: " << encryption_time.count() << " ms"
            << std::endl;

  // decrypt
  ifile.open(crypt_path + dst_filename, iflag);
  ofile.open(crypt_path + src_filename_ex, oflag);
  ifile.read(reinterpret_cast<char*>(iv.data()), iv.size());
  ibuffer.resize(ibuf_size);

  // Start timing for decryption
  auto start_decrypt = std::chrono::high_resolution_clock::now();

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

  // End timing for decryption and output the time taken
  auto end_decrypt = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> decryption_time =
    end_decrypt - start_decrypt;
  std::cout << "Decryption time: " << decryption_time.count() << " ms"
            << std::endl;

  // compare
  std::string cmpCommand = "cmp --silent " + test_files + src_filename + " " +
                           crypt_path + src_filename_ex;  // NOLINT
  if (system(cmpCommand.c_str()) != 0) {                  // NOLINT
    std::cout << "文件 " << test_files + src_filename << " 和 "
              << crypt_path + src_filename_ex << " 不同" << std::endl;
    return false;
  }
  return true;
}

class EncryptAndDecryptSmall : public ::testing::Test {
 protected:
  void SetUp() override { fs::remove_all(crypt_path); }

  void TearDown() override { fs::remove_all(crypt_path); }
};

TEST_F(EncryptAndDecryptSmall, EncryptAndDecryptSmall) {  // NOLINT
  ASSERT_TRUE(test("/text/small_text_test_file"));
}

}  // namespace zipfiles::server
