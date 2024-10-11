
#include "server/crypto/crypto.h"
#include <crypto++/filters.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace zipfiles::server {

constexpr int CRYPTO_BLOCK_SIZE = 1 << 20;

AESEncryptor::AESEncryptor(const std::string& key) : key(generateKey(key)) {}

std::string AESEncryptor::generateKey(const std::string& rawKey) {
  SHA256 hash;
  std::string digest;
  StringSource ss(
    rawKey, true, new HashFilter(hash, new HexEncoder(new StringSink(digest)))
  );
  return digest.substr(0, 32);  // 32 字节作为密钥
}

CryptStatus AESEncryptor::encryptFile(
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
        CBC_Mode<AES>::Encryption encryption(
          reinterpret_cast<CryptoPP::byte*>(key.data()), key.size(), iv.data()
        );
        outputData.clear();
        ArraySource as(
          ibuffer.data(), ibuffer.size(), true,
          new StreamTransformationFilter(
            encryption, new VectorSink(outputData),
            (ibuffer.size() != CRYPTO_BLOCK_SIZE && flush)
              ? StreamTransformationFilter::PKCS_PADDING
              : StreamTransformationFilter::NO_PADDING
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

CryptStatus AESEncryptor::decryptFile(
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
            (ibuffer.size() != CRYPTO_BLOCK_SIZE && flush)
              ? StreamTransformationFilter::PKCS_PADDING
              : StreamTransformationFilter::NO_PADDING
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

}  // namespace zipfiles::server