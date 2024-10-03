#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "server/backup/backup.h"
#include "server/pack/unpack.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

/**
 * @brief 解压缩存档文件到指定的输出目录。
 *
 * @param archivePath 存档文件的路径。
 *
 * @param outputDir 文件解压到的输出目录路径。
 *
 * ! Deprecated
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

/**
 * @brief 给定一个打包过后的数据流，恢复成多个文件的数据流
 *
 * @param packedData 打包后的数据流
 *
 * @param dst 要恢复到的目录
 */
void unpackFiles(const std::vector<uint8_t>& packedData, const fs::path& dst) {
  size_t offset = 0;
  try {
    while (offset < packedData.size()) {
      // 读取路径
      uint32_t pathSize = 0;
      std::memcpy(&pathSize, packedData.data() + offset, sizeof(pathSize));
      offset += sizeof(pathSize);

      std::string relativePath(
        packedData.data() + offset, packedData.data() + offset + pathSize
      );
      offset += pathSize;

      // 读取文件
      uint32_t fileSize = 0;
      std::memcpy(&fileSize, packedData.data() + offset, sizeof(fileSize));
      offset += sizeof(fileSize);

      std::vector<uint8_t> fileData(
        packedData.data() + offset, packedData.data() + offset + fileSize
      );
      offset += fileSize;

      // 创建相关文件夹并写回
      fs::path filePath = dst / relativePath;
      fs::create_directories(filePath.parent_path());
      writeFile(filePath, fileData);
    }
  } catch (const std::exception& e) {
    throw std::runtime_error(
      "Failed to unpack files: " + std::string(e.what())
    );
  }
}

}  // namespace zipfiles::server
