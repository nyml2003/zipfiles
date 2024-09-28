#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace zipfiles::server {

namespace fs = std::filesystem;
/**
 * @brief
 */
void unpackArchive(
  const std::string& archivePath,
  const std::string& outputDir
) {
  std::ifstream archive(archivePath, std::ios::binary);
  if (!archive) {
    throw std::runtime_error("Failed to open archive: " + archivePath);
  }

  while (archive) {
    size_t pathLength;
    archive.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
    if (archive.eof()) {
      break;
    }

    // 读取路径字符串
    std::string pathStr(pathLength, '\0');
    archive.read(pathStr.data(), pathLength);

    // 读取文件大小
    size_t fileSize = 0;
    archive.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

    // 生成输出路径
    fs::path outputPath = fs::path(outputDir) / fs::path(pathStr).filename();
    // 打开输出文件
    std::ofstream file(outputPath, std::ios::binary);
    if (!file) {
      throw std::runtime_error("Failed to create file: " + outputPath.string());
    }

    // 缓冲区
    std::array<char, 8192> buffer{};
    size_t remaining_size = fileSize;
    // 把文件内容导出
    while (remaining_size > 0) {
      size_t readSize = std::min(sizeof(buffer), remaining_size);
      archive.read(buffer.data(), readSize);
      file.write(buffer.data(), readSize);
      remaining_size -= readSize;
    }
  }
}
}  // namespace zipfiles::server
