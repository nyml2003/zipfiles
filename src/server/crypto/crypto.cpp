
#include "server/crypto/crypto.h"
#include <crypto++/filters.h>
#include <cstdint>
#include <string>
#include <vector>

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
  const std::vector<uint8_t>& inputData,
  const std::array<CryptoPP::byte, AES::BLOCKSIZE>& iv
) {
  try {
    std::vector<uint8_t> outputData;

    CBC_Mode<AES>::Encryption encryption(
      reinterpret_cast<CryptoPP::byte*>(key.data()), key.size(), iv.data()
    );

    ArraySource as(
      inputData.data(), inputData.size(), true,
      new StreamTransformationFilter(
        encryption, new VectorSink(outputData),
        StreamTransformationFilter::PKCS_PADDING
      )
    );

    return outputData;
  } catch (const CryptoPP::Exception& e) {
    throw std::runtime_error("Encryption failed, " + std::string(e.what()));
  } catch (const std::exception& e) {
    throw std::runtime_error("An error occurred, " + std::string(e.what()));
  }
}

std::vector<uint8_t> AESEncryptor::decryptFile(
  const std::vector<uint8_t>& inputData,
  const std::array<CryptoPP::byte, AES::BLOCKSIZE>& iv
) {
  try {
    std::vector<uint8_t> outputData;

    CBC_Mode<AES>::Decryption decryption(
      reinterpret_cast<CryptoPP::byte*>(key.data()), key.size(), iv.data()
    );

    ArraySource as(
      inputData.data(), inputData.size(),
      true,
      new StreamTransformationFilter(
        decryption, new VectorSink(outputData),
        StreamTransformationFilter::PKCS_PADDING
      )
    );

    return outputData;
  } catch (const CryptoPP::Exception& e) {
    throw std::runtime_error("Failed to decrypt, " + std::string(e.what()));
  } catch (const std::exception& e) {
    throw std::runtime_error("An error occurred, " + std::string(e.what()));
  }
}

}  // namespace zipfiles::server