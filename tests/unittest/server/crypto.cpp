#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace CryptoPP;
using namespace std;

void ReadFile(const string& filename, vector<uint8_t>& data) {
  ifstream file(filename, ios::binary);
  if (file.is_open()) {
    file.seekg(0, ios::end);
    size_t size = file.tellg();
    file.seekg(0, ios::beg);
    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    file.close();
  } else {
    cerr << "无法打开文件: " << filename << endl;
  }
}

void WriteFile(const string& filename, const vector<uint8_t>& data) {
  ofstream file(filename, ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
  } else {
    cerr << "无法写入文件: " << filename << endl;
  }
}

int main() {
  // 从文件读取明文数据
  vector<uint8_t> plaintext;
  ReadFile("/app/test.txt", plaintext);

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

  // 将密文写入文件
  WriteFile("/app/ciphertext.bin", ciphertext);

  // 打印加密后的密文
  cout << "密文：";
  for (auto c : ciphertext) {
    cout << hex << (int)c;
  }
  cout << endl;

  // 从文件读取密文数据
  vector<uint8_t> readCiphertext;
  ReadFile("/app/ciphertext.bin", readCiphertext);

  // 解密过程
  CBC_Mode<AES>::Decryption decryption(
    (CryptoPP::byte*)key.c_str(), key.length(), iv
  );
  vector<uint8_t> decryptedtext;

  ArraySource as(
    readCiphertext.data(), readCiphertext.size(), true,
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