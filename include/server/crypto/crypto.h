#ifndef ZIPFILES_SERVER_ENCRYPT_H
#define ZIPFILES_SERVER_ENCRYPT_H

#include <crypto++/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/crc.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace zipfiles::server {

using CryptoPP::AES;
using CryptoPP::ArraySource;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::CBC_Mode;
using CryptoPP::CRC32;
using CryptoPP::HashFilter;
using CryptoPP::HexEncoder;
using CryptoPP::SHA256;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::StringSink;
using CryptoPP::VectorSink;

constexpr int CRYPTO_BLOCK_SIZE = 1 << 20;

struct CryptStatus {
  bool flush;
  bool lack;
  std::vector<uint8_t>* obuffer;
};

class Cryptor {
 public:
  explicit Cryptor(const std::string& key);

  CryptStatus encryptFile(
    const std::vector<uint8_t>& inputData,
    const std::array<CryptoPP::byte, AES::BLOCKSIZE>& iv,
    bool flush
  );

  CryptStatus decryptFile(
    const std::vector<uint8_t>& inputData,
    const std::array<CryptoPP::byte, AES::BLOCKSIZE>& iv,
    bool flush
  );

  static std::string encodeKey(const std::string& key);

  static bool checkKey(const std::string& encodedKey, const std::string& key);

 private:
  std::string key;
  static std::string generateKey(const std::string& rawKey);
};

class CRC {
 public:
  void update(const std::vector<uint8_t>& data);
  std::vector<uint8_t> getChecksum();
  static bool check(const std::string& filename);

 private:
  CRC32 crc;
};

}  // namespace zipfiles::server
#endif  // !ZIPFILES_SERVER_ENCRYPT_H