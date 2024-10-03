#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "server/backup/backup.h"
#include "server/pack/pack.h"

namespace zipfiles::server {

/**
 * @brief 给定一个打包后的文件名(路径形式)，返回一个archive实例
 *
 * ! Deprecated
 *
 */
std::ofstream createArchive(const std::string& archiveName) {
  std::ofstream archive(archiveName, std::ios::binary | std::ios::app);
  if (!archive) {
    throw std::runtime_error("Failed to create archive: " + archiveName);
  }

  return archive;
}

/**
 * @brief 传入目标流和文件路径，将未压缩的文件流拼接到目标流中
 *
 * ! Deprecated
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
 * @brief 传入文件路径数组，返回一个打包后的数据流
 *
 * @param files 一个文件路径数组，要求是去除了最近公共祖先后的相对路径
 *
 */
std::vector<uint8_t> packFiles(const std::vector<fs::path>& files) {
  std::vector<uint8_t> packedData;
  try {
    for (const auto& file : files) {
      std::vector<uint8_t> fileData = readFile(file);
      uint32_t fileSize = fileData.size();
      std::string relativePath = file.string();
      uint32_t pathSize = relativePath.size();

      // 插入相对路径大小
      packedData.insert(
        packedData.end(), reinterpret_cast<uint8_t*>(&pathSize),
        reinterpret_cast<uint8_t*>(&pathSize) + sizeof(pathSize)
      );
      // 插入相对路径
      packedData.insert(
        packedData.end(), relativePath.begin(), relativePath.end()
      );
      // 插入文件大小
      packedData.insert(
        packedData.end(), reinterpret_cast<uint8_t*>(&fileSize),
        reinterpret_cast<uint8_t*>(&fileSize) + sizeof(fileSize)
      );
      // 插入文件数据
      packedData.insert(packedData.end(), fileData.begin(), fileData.end());
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to pack files: " + std::string(e.what()));
  }

  return packedData;
}

}  // namespace zipfiles::server
