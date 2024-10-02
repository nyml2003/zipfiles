#include <gtest/gtest.h>
#include <fstream>
#include "server/crypto/crypto.h"

namespace zipfiles::server {

class AESEncryptorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a sample input file
    std::ofstream inputFile(inputFilePath);
    inputFile << "This is a test file for encryption.";
    inputFile.close();
  }

  void TearDown() override {
    // Clean up files
    std::remove(inputFilePath.c_str());
    std::remove(encryptedFilePath.c_str());
    std::remove(decryptedFilePath.c_str());
  }

  const std::string key = "0123456789abcdef";  // 16-byte key for AES-128
  const std::string inputFilePath = "test_input.txt";
  const std::string encryptedFilePath = "test_encrypted.txt";
  const std::string decryptedFilePath = "test_decrypted.txt";
};

TEST_F(AESEncryptorTest, EncryptFile) {
  AESEncryptor encryptor(key);
  EXPECT_NO_THROW(encryptor.encryptFile(inputFilePath, encryptedFilePath));

  // Check if the encrypted file exists
  std::ifstream encryptedFile(encryptedFilePath);
  EXPECT_TRUE(encryptedFile.good());
}

TEST_F(AESEncryptorTest, DecryptFile) {
  AESEncryptor encryptor(key);
  encryptor.encryptFile(inputFilePath, encryptedFilePath);
  EXPECT_NO_THROW(encryptor.decryptFile(encryptedFilePath, decryptedFilePath));

  // Check if the decrypted file exists
  std::ifstream decryptedFile(decryptedFilePath);
  EXPECT_TRUE(decryptedFile.good());

  // Check if the decrypted content matches the original content
  std::ifstream originalFile(inputFilePath);
  std::string originalContent(
    (std::istreambuf_iterator<char>(originalFile)),
    std::istreambuf_iterator<char>()
  );
  std::string decryptedContent(
    (std::istreambuf_iterator<char>(decryptedFile)),
    std::istreambuf_iterator<char>()
  );
  EXPECT_EQ(originalContent, decryptedContent);
}

TEST_F(AESEncryptorTest, EncryptAndDecryptFile) {
  AESEncryptor encryptor(key);
  encryptor.encryptFile(inputFilePath, encryptedFilePath);
  encryptor.decryptFile(encryptedFilePath, decryptedFilePath);

  // Check if the decrypted content matches the original content
  std::ifstream originalFile(inputFilePath);
  std::ifstream decryptedFile(decryptedFilePath);
  std::string originalContent(
    (std::istreambuf_iterator<char>(originalFile)),
    std::istreambuf_iterator<char>()
  );
  std::string decryptedContent(
    (std::istreambuf_iterator<char>(decryptedFile)),
    std::istreambuf_iterator<char>()
  );
  EXPECT_EQ(originalContent, decryptedContent);
}

}  // namespace zipfiles::server

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}