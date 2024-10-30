#include "server/crypto/crypto.h"

#include <crypto++/filters.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using CryptoPP::CBC_Mode;
using CryptoPP::HashFilter;
using CryptoPP::HexEncoder;
using CryptoPP::SHA256;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::StringSource;

namespace zipfiles::server {

constexpr int CRYPTO_BLOCK_SIZE = 1 << 20;

Cryptor::Cryptor(const std::string& key) : key(generateKey(key)) {}

std::string Cryptor::generateKey(const std::string& rawKey) {
  SHA256 hash;
  std::string digest;
  StringSource ss(
    rawKey, true, new HashFilter(hash, new HexEncoder(new StringSink(digest)))
  );
  return digest.substr(0, 32);  // 32 字节作为密钥
}

CryptStatus Cryptor::encryptFile(
  const std::vector<uint8_t>& inputData,
  const std::array<CryptoPP::byte, AES::BLOCKSIZE>& iv,
  bool flush
) {
  static thread_local std::vector<uint8_t> ibuffer;
  static thread_local std::vector<uint8_t> outputData;
  static thread_local size_t start = 0;

  auto append_buf = [&](size_t len) {
    ibuffer.insert(
      ibuffer.end(),
      inputData.begin() +
        static_cast<std::vector<uint8_t>::difference_type>(start),
      inputData.begin() +
        static_cast<std::vector<uint8_t>::difference_type>(start + len)
    );
    start += len;
  };

  bool lack = (start == inputData.size());

  if (!lack) {
    append_buf(
      std::min(CRYPTO_BLOCK_SIZE - 1 - ibuffer.size(), inputData.size() - start)
    );
    lack = (start == inputData.size());
    start %= inputData.size();
    if (ibuffer.size() == CRYPTO_BLOCK_SIZE - 1 || flush) {
      try {
        CBC_Mode<AES>::Encryption encryption(
          reinterpret_cast<CryptoPP::byte*>(key.data()), key.size(), iv.data()
        );
        outputData.clear();
        ArraySource as(
          ibuffer.data(), ibuffer.size(), true,
          new StreamTransformationFilter(
            encryption, new VectorSink(outputData),
            StreamTransformationFilter::PKCS_PADDING
          )
        );
        assert(outputData.size() == CRYPTO_BLOCK_SIZE || flush);
        ibuffer.clear();
        return {true, lack, &outputData};
      } catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Encryption failed, " + std::string(e.what()));
      } catch (const std::exception& e) {
        throw std::runtime_error("An error occurred, " + std::string(e.what()));
      }
    }
  }
  start = 0;
  return {false, lack, &outputData};
}

CryptStatus Cryptor::decryptFile(
  const std::vector<uint8_t>& inputData,
  const std::array<CryptoPP::byte, AES::BLOCKSIZE>& iv,
  bool flush
) {
  static thread_local std::vector<uint8_t> ibuffer;
  static thread_local std::vector<uint8_t> outputData;
  static thread_local size_t start = 0;

  auto append_buf = [&](size_t len) {
    ibuffer.insert(
      ibuffer.end(),
      inputData.begin() +
        static_cast<std::vector<uint8_t>::difference_type>(start),
      inputData.begin() +
        static_cast<std::vector<uint8_t>::difference_type>(start + len)
    );
    start += len;
  };

  bool lack = (start == inputData.size());

  if (!lack) {
    append_buf(
      std::min(CRYPTO_BLOCK_SIZE - ibuffer.size(), inputData.size() - start)
    );
    lack = (start == inputData.size());
    start %= inputData.size();
    if (ibuffer.size() == CRYPTO_BLOCK_SIZE || flush) {
      try {
        CBC_Mode<AES>::Decryption decryption(
          reinterpret_cast<CryptoPP::byte*>(key.data()), key.size(), iv.data()
        );
        outputData.clear();
        ArraySource as(
          ibuffer.data(), ibuffer.size(), true,
          new StreamTransformationFilter(
            decryption, new VectorSink(outputData),
            StreamTransformationFilter::PKCS_PADDING
          )
        );
        assert(outputData.size() == CRYPTO_BLOCK_SIZE || flush);
        ibuffer.clear();
        return {true, lack, &outputData};
      } catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Failed to decrypt, " + std::string(e.what()));
      } catch (const std::exception& e) {
        throw std::runtime_error("An error occurred, " + std::string(e.what()));
      }
    }
  }
  start = 0;
  return {false, lack, &outputData};
}

void CRC::update(const std::vector<uint8_t>& data) {
  crc.Update(data.data(), data.size());
}

std::vector<uint8_t> CRC::getChecksum() {
  std::vector<uint8_t> checksum(CRC32::DIGESTSIZE);
  crc.Final(checksum.data());
  return checksum;
}

bool CRC::check(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file " + filename);
  }

  std::vector<uint8_t> checksum0(CRC32::DIGESTSIZE);
  file.read(
    reinterpret_cast<char*>(checksum0.data()),
    static_cast<std::streamsize>(checksum0.size())
  );

  CRC32 crc;

  std::vector<uint8_t> buffer(CRYPTO_BLOCK_SIZE);
  while (!file.eof()) {
    file.read(
      reinterpret_cast<char*>(buffer.data()),
      static_cast<std::streamsize>(buffer.size())
    );
    crc.Update(buffer.data(), file.gcount());
  }

  std::vector<uint8_t> checksum(CRC32::DIGESTSIZE);
  crc.Final(checksum.data());

  return checksum == checksum0;
}
}  // namespace zipfiles::server