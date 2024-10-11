#ifndef ZIPFILES_SERVER_ENCRYPT_H
#define ZIPFILES_SERVER_ENCRYPT_H

#include <crypto++/filters.h>
#include <cryptopp/aes.h>
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
using CryptoPP::ArraySink;
using CryptoPP::ArraySource;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::CBC_Mode;
using CryptoPP::HashFilter;
using CryptoPP::HexEncoder;
using CryptoPP::SHA256;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::VectorSink;

struct CryptStatus {
  bool flush;
  bool lack;
  std::vector<uint8_t>* obuffer;
};

class AESEncryptor {
 public:
  explicit AESEncryptor(const std::string& key);

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

 private:
  std::string key;
  static std::string generateKey(const std::string& rawKey);
};

}  // namespace zipfiles::server
#endif  // !ZIPFILES_SERVER_ENCRYPT_H