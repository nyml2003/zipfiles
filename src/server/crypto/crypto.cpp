#include <crypto++/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cstdint>
#include <string>
#include <vector>
#include "server/crypto/crypto.h"

using CryptoPP::AES;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::byte;
using CryptoPP::CBC_Mode;
using CryptoPP::HashFilter;
using CryptoPP::HexEncoder;
using CryptoPP::SHA256;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::VectorSink;

namespace zipfiles::server {

AESEncryptor::AESEncryptor(const std::string& key) : key(generateKey(key)) {}

std::string AESEncryptor::generateKey(const std::string& rawKey) {
  SHA256 hash;
  std::string digest;
  StringSource ss(
    rawKey, true, new HashFilter(hash, new HexEncoder(new StringSink(digest)))
  );
  return digest.substr(0, 32);  // 取前 32 字节作为密钥
}

std::vector<uint8_t> AESEncryptor::encryptFile(
  const std::vector<uint8_t>& inputData
) {
  try {
    std::vector<uint8_t> outputData;
    std::array<byte, AES::BLOCKSIZE> iv{};
    AutoSeededRandomPool prng;
    prng.GenerateBlock(iv.data(), iv.size());

    CBC_Mode<AES>::Encryption encryption(
      reinterpret_cast<byte*>(key.data()), key.size(), iv.data()
    );

    // 将 IV 添加到输出数据的开头
    outputData.insert(outputData.end(), iv.begin(), iv.end());

    StringSource ss(
      inputData.data(), inputData.size(), true,
      new StreamTransformationFilter(encryption, new VectorSink(outputData))
    );

    return outputData;
  } catch (const CryptoPP::Exception& e) {
    throw std::runtime_error("Encryption failed, " + std::string(e.what()));
  } catch (const std::exception& e) {
    throw std::runtime_error("An error occurred, " + std::string(e.what()));
  }
}

std::vector<uint8_t> AESEncryptor::decryptFile(
  const std::vector<uint8_t>& inputData
) {
  try {
    std::vector<uint8_t> outputData;
    std::array<byte, AES::BLOCKSIZE> iv{};

    // 确保输入数据至少包含一个块大小的 IV
    if (inputData.size() < AES::BLOCKSIZE) {
      throw std::runtime_error("Data too short");
    }

    // 提取 IV
    std::copy(
      inputData.begin(), inputData.begin() + AES::BLOCKSIZE, iv.begin()
    );

    CBC_Mode<AES>::Decryption decryption(
      reinterpret_cast<byte*>(key.data()), key.size(), iv.data()
    );

    StringSource ss(
      inputData.data() + AES::BLOCKSIZE, inputData.size() - AES::BLOCKSIZE,
      true,
      new StreamTransformationFilter(decryption, new VectorSink(outputData))
    );

    return outputData;
  } catch (const CryptoPP::Exception& e) {
    throw std::runtime_error("Failed to decrypt, " + std::string(e.what()));
  } catch (const std::exception& e) {
    throw std::runtime_error("An error occurred, " + std::string(e.what()));
  }
}

}  // namespace zipfiles::server