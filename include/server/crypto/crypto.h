#ifndef ZIPFILES_SERVER_ENCRYPT_H
#define ZIPFILES_SERVER_ENCRYPT_H

#include <string>

namespace zipfiles::server {

class AESEncryptor {
 public:
  explicit AESEncryptor(const std::string& key);
  void encryptFile(
    const std::string& inputFilePath,
    const std::string& outputFilePath
  );
  void decryptFile(
    const std::string& inputFilePath,
    const std::string& outputFilePath
  );

 private:
  std::string key;
};

}  // namespace zipfiles::server
#endif  // !ZIPFILES_SERVER_ENCRYPT_H