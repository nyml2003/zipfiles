#ifndef ZIPFILES_SERVER_ENCRYPT_H
#define ZIPFILES_SERVER_ENCRYPT_H

#include <cstdint>
#include <string>
#include <vector>

namespace zipfiles::server {

class AESEncryptor {
 public:
  explicit AESEncryptor(const std::string& key);
  std::vector<uint8_t> encryptFile(const std::vector<uint8_t>& inputData);
  std::vector<uint8_t> decryptFile(const std::vector<uint8_t>& inputData);

 private:
  std::string key;
  static std::string generateKey(const std::string& rawKey);
};

}  // namespace zipfiles::server
#endif  // !ZIPFILES_SERVER_ENCRYPT_H