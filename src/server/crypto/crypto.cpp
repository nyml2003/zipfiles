#include <cryptopp/aes.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <string>
#include "server/crypto/crypto.h"

namespace zipfiles::server {

AESEncryptor::AESEncryptor(const std::string& key) : key(key) {}

void AESEncryptor::encryptFile(
  const std::string& inputFilePath,
  const std::string& outputFilePath
) {
  using namespace CryptoPP;

  byte iv[AES::BLOCKSIZE];
  AutoSeededRandomPool prng;
  prng.GenerateBlock(iv, sizeof(iv));

  CBC_Mode<AES>::Encryption encryption((byte*)key.data(), key.size(), iv);

  FileSource temp(
    inputFilePath.c_str(), true,
    new StreamTransformationFilter(
      encryption, new FileSink(outputFilePath.c_str())
    )
  );
}

void AESEncryptor::decryptFile(
  const std::string& inputFilePath,
  const std::string& outputFilePath
) {
  using namespace CryptoPP;

  byte iv[AES::BLOCKSIZE];
  AutoSeededRandomPool prng;
  prng.GenerateBlock(iv, sizeof(iv));

  CBC_Mode<AES>::Decryption decryption(
    reinterpret_cast<byte*>(key.data()), key.size(), iv
  );

  FileSource temp(
    inputFilePath.c_str(), true,
    new StreamTransformationFilter(
      decryption, new FileSink(outputFilePath.c_str())
    )
  );
}

}  // namespace zipfiles::server