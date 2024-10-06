#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <iostream>
#include <vector>

using namespace CryptoPP;
using namespace std;

int main() {
  // 明文数据块
  vector<uint8_t> plaintext = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'C',
                               'r', 'y', 'p', 't', 'o', '+', '+'};
  string key = "1234567890123456";  // AES-128密钥长度为16字节

  // 打印明文
  cout << "明文：";
  for (auto c : plaintext) {
    cout << c;
  }
  cout << endl;

  // 加密过程
  CryptoPP::byte iv[AES::BLOCKSIZE];
  memset(iv, 0x00, AES::BLOCKSIZE);
  CBC_Mode<AES>::Encryption encryption(
    (CryptoPP::byte*)key.c_str(), key.length(), iv
  );
  vector<uint8_t> ciphertext;

  ArraySource(
    plaintext.data(), plaintext.size(), true,
    new StreamTransformationFilter(
      encryption, new VectorSink(ciphertext),
      StreamTransformationFilter::PKCS_PADDING
    )
  );

  // 打印加密后的密文
  cout << "密文：";
  for (auto c : ciphertext) {
    cout << hex << (int)c;
  }
  cout << endl;

  // 解密过程
  CBC_Mode<AES>::Decryption decryption(
    (CryptoPP::byte*)key.c_str(), key.length(), iv
  );
  vector<uint8_t> decryptedtext;

  ArraySource(
    ciphertext.data(), ciphertext.size(), true,
    new StreamTransformationFilter(
      decryption, new VectorSink(decryptedtext),
      StreamTransformationFilter::PKCS_PADDING
    )
  );

  // 打印解密后的明文
  cout << "解密后的明文：";
  for (auto c : decryptedtext) {
    cout << c;
  }
  cout << endl;

  return 0;
}
