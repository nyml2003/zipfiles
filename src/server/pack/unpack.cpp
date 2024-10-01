#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace zipfiles::server {

/**
 * @brief 解压缩存档文件到指定的输出目录。
 *
 * @param archivePath 存档文件的路径。
 *
 * @param outputDir 文件解压到的输出目录路径。
 *
 */
void unpackArchive(const fs::path& archivePath, const fs::path& outputDir) {
  std::ifstream archive(archivePath, std::ios::binary);
  if (!archive) {
    throw std::runtime_error("Unable to open " + archivePath.string());
  }

  // 如果输出目录不存在，则创建
  if (!fs::exists(outputDir)) {
    fs::create_directories(outputDir);
  }

  while (archive) {
    size_t pathLength = 0;
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
    fs::path outputPath = outputDir / fs::path(pathStr).filename();
    // 打开输出文件
    std::ofstream file(outputPath, std::ios::binary);
    if (!file) {
      throw std::runtime_error("无法创建文件: " + outputPath.string());
    }

    // 文件内容缓冲区
    std::array<char, 8192> buffer{};
    size_t remaining_size = fileSize;
    // 导出文件内容
    while (remaining_size > 0) {
      size_t readSize = std::min(sizeof(buffer), remaining_size);
      archive.read(buffer.data(), readSize);
      file.write(buffer.data(), readSize);
      remaining_size -= readSize;
    }
  }
}

}  // namespace zipfiles::server
