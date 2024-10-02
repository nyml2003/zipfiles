#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "server/pack/pack.h"

namespace zipfiles::server {
/**
 * @brief 传入目标流和文件路径，将未压缩的文件流拼接到目标流中
 *
 */
void packFileToArchive(std::ofstream& archive, const fs::path& filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filePath.string());
  }

  // 写入文件路径长度和路径
  std::string pathStr = filePath.string();
  size_t pathLength = pathStr.size();
  archive.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
  archive.write(pathStr.c_str(), pathLength);

  // 写入文件大小和内容
  file.seekg(0, std::ios::end);
  size_t file_size = file.tellg();
  file.seekg(0, std::ios::beg);
  archive.write(reinterpret_cast<const char*>(&file_size), sizeof(file_size));

  std::array<char, 8192> buffer{};
  while (file.read(buffer.data(), sizeof(buffer))) {
    archive.write(buffer.data(), file.gcount());
  }
  archive.write(buffer.data(), file.gcount());
}

/**
 * @brief 给定一个打包后的文件名(路径形式)，返回一个archive实例
 *
 */
std::ofstream createArchive(const std::string& archiveName) {
  std::ofstream archive(archiveName, std::ios::binary | std::ios::app);
  if (!archive) {
    throw std::runtime_error("Failed to create archive: " + archiveName);
  }

  return archive;
}

}  // namespace zipfiles::server
