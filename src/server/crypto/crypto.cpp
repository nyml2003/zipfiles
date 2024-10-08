#include "server/crypto/crypto.h"
#include <cryptopp/aes.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <memory>
#include <string>

using CryptoPP::AES;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::byte;
using CryptoPP::CBC_Mode;
using CryptoPP::FileSink;
using CryptoPP::FileSource;
using CryptoPP::Redirector;
using CryptoPP::StreamTransformationFilter;

namespace zipfiles::server {

AESEncryptor::AESEncryptor(const std::string& key) : key(key) {}

void AESEncryptor::encryptFile(
  const std::string& inputFilePath,
  const std::string& outputFilePath
) {
  std::string tempOutputFilePath = outputFilePath + ".tmp";

  try {
    std::array<byte, AES::BLOCKSIZE> iv{};
    AutoSeededRandomPool prng;
    prng.GenerateBlock(iv.data(), iv.size());

    CBC_Mode<AES>::Encryption encryption(
      reinterpret_cast<byte*>(key.data()), key.size(), iv.data()
    );

    FileSink file(tempOutputFilePath.c_str());
    file.Put(iv.data(), iv.size());  // 写入 IV

    FileSource temp(
      inputFilePath.c_str(), true,
      new StreamTransformationFilter(encryption, new Redirector(file))
    );

    // 替换原文件
    if (std::rename(tempOutputFilePath.c_str(), outputFilePath.c_str()) != 0) {
      throw std::runtime_error("Failed to rename temporary file to output file"
      );
    }
  } catch (const CryptoPP::Exception& e) {
    throw std::runtime_error("Encryption failed, " + std::string(e.what()));
  } catch (const std::exception& e) {
    throw std::runtime_error("An error occurred, " + std::string(e.what()));
  }
}

void AESEncryptor::decryptFile(
  const std::string& inputFilePath,
  const std::string& outputFilePath
) {
  std::string tempOutputFilePath = outputFilePath + ".tmp";

  try {
    std::array<byte, AES::BLOCKSIZE> iv{};

    FileSource file(inputFilePath.c_str(), false);
    file.Pump(iv.size());  // 读取 IV
    file.Get(iv.data(), iv.size());

    CBC_Mode<AES>::Decryption decryption(
      reinterpret_cast<byte*>(key.data()), key.size(), iv.data()
    );

    file.Attach(new StreamTransformationFilter(
      decryption, new FileSink(tempOutputFilePath.c_str())
    ));

    // !使用智能指针会segment fault: double free or corruption
    // auto fileSink = std::make_shared<FileSink>(tempOutputFilePath.c_str());
    // auto filter =
    //   std::make_shared<StreamTransformationFilter>(decryption,
    //   fileSink.get());

    // file.Attach(filter.get());
    file.PumpAll();

    // 替换原文件
    if (std::rename(tempOutputFilePath.c_str(), outputFilePath.c_str()) != 0) {
      throw std::runtime_error("Failed to rename temporary file to output file"
      );
    }
  } catch (const CryptoPP::Exception& e) {
    throw std::runtime_error("Encryption failed, " + std::string(e.what()));
  } catch (const std::exception& e) {
    throw std::runtime_error("An error occurred, " + std::string(e.what()));
  }
}

}  // namespace zipfiles::server