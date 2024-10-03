#include <gtest/gtest.h>
#include <fstream>
#include "server/crypto/crypto.h"

namespace zipfiles::server {

class AESEncryptorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    key = "testkey123451250p8asd";
    encryptor = std::make_unique<AESEncryptor>(key);
  }

  std::string key;
  std::unique_ptr<AESEncryptor> encryptor;
};

TEST_F(AESEncryptorTest, EncryptDecrypt) {
  std::vector<uint8_t> inputData = {'H', 'e', 'l', 'l', 'o', ' ',
                                    'W', 'o', 'r', 'l', 'd'};
  std::vector<uint8_t> encryptedData = encryptor->encryptFile(inputData);
  std::vector<uint8_t> decryptedData = encryptor->decryptFile(encryptedData);

  EXPECT_EQ(inputData, decryptedData);
}

TEST_F(AESEncryptorTest, EncryptDifferentOutputs) {
  std::vector<uint8_t> inputData = {'H', 'e', 'l', 'l', 'o', ' ',
                                    'W', 'o', 'r', 'l', 'd'};
  std::vector<uint8_t> encryptedData1 = encryptor->encryptFile(inputData);
  std::vector<uint8_t> encryptedData2 = encryptor->encryptFile(inputData);

  EXPECT_NE(encryptedData1, encryptedData2);
}

TEST_F(AESEncryptorTest, DecryptInvalidData) {
  std::vector<uint8_t> invalidData = {'I', 'n', 'v', 'a', 'l', 'i', 'd'};
  EXPECT_THROW(encryptor->decryptFile(invalidData), std::runtime_error);
}

TEST_F(AESEncryptorTest, DecryptWithWrongKey) {
  std::vector<uint8_t> inputData = {'H', 'e', 'l', 'l', 'o', ' ',
                                    'W', 'o', 'r', 'l', 'd'};
  std::vector<uint8_t> encryptedData = encryptor->encryptFile(inputData);

  // 使用不同的密钥创建一个新的 AESEncryptor 实例
  std::string wrongKey = "wrongkey";
  AESEncryptor wrongEncryptor(wrongKey);

  // 尝试使用错误的密钥解密数据，应该抛出运行时错误
  EXPECT_THROW(wrongEncryptor.decryptFile(encryptedData), std::runtime_error);
}

}  // namespace zipfiles::server