#include "server/pack/pack.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>
#include "mp/dto.h"
#include "server/backup/backup.h"
#include "server/tools/fsapi.h"

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
  archive.write(pathStr.c_str(), static_cast<std::streamsize>(pathLength));

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
 * @brief 从给定的输入流读入指定大小的chunk
 *
 * @param inFile 输入流
 *
 * @param chunkSize 指定的大小，单位是byte
 *
 */
bool readChunk(
  std::vector<uint8_t>& buffer,
  std::ifstream& inFile,
  size_t chunkSize
) {
  if (!inFile.is_open()) {
    throw std::runtime_error("File is not open");
  }

  std::vector<uint8_t> buffers(chunkSize);
  try {
    inFile.read(
      reinterpret_cast<char*>(buffer.data()),
      static_cast<std::streamsize>(buffer.size())
    );

    if (inFile.bad()) {
      throw std::runtime_error("Failed to read data from stream");
    }

    buffer.resize(inFile.gcount());
  } catch (const std::ios_base::failure& e) {
    throw std::runtime_error("Failed to read file: " + std::string(e.what()));
  }

  return true;
}

/**
 * @brief 根据文件的路径以及数据大小，返回其header
 *
 * @param filePath 文件路径(相对)
 *
 * @param dataSize 文件大小
 *
 */
size_t insertHeader(
  const fs::path& filePath,
  size_t dataSize,
  std::vector<uint8_t>& target,
  size_t targetOffset
) {
  size_t relativePathSize = filePath.string().size();
  size_t totalSize = sizeof(size_t) * 2 + relativePathSize;

  std::cout << "Creating header for: " << filePath
            << " totalSize is: " << totalSize << std::endl;

  // 写入文件路径大小
  target.insert(
    target.begin() + static_cast<std::ptrdiff_t>(targetOffset),
    reinterpret_cast<uint8_t*>(&relativePathSize),
    reinterpret_cast<uint8_t*>(&relativePathSize) + sizeof(size_t)
  );

  targetOffset += sizeof(size_t);

  std::string filePathStr = filePath.string();
  // 写入文件路径，这里需要一个临时变量来获得一致的地址
  target.insert(
    target.begin() + static_cast<std::ptrdiff_t>(targetOffset),
    filePathStr.begin(), filePathStr.end()
  );

  targetOffset += relativePathSize;

  // 写入文件数据大小
  target.insert(
    target.begin() + static_cast<std::ptrdiff_t>(targetOffset),
    reinterpret_cast<uint8_t*>(&dataSize),
    reinterpret_cast<uint8_t*>(&dataSize) + sizeof(size_t)
  );

  return totalSize;
}

/**
 * @brief
 * 传入文件路径数组，返回一个打包后的数据块(第一个块会带有额外信息以及部分文件数据)
 *
 * @param files 一个文件路径数组
 *
 */
std::pair<bool, std::vector<uint8_t>&>
packFilesByBlock(const std::vector<fs::path>& files, bool flush) {
  static thread_local std::vector<uint8_t> obuffer;  // 默认为1<<19
  static thread_local size_t currentFileIndex = 0;
  static thread_local std::ifstream inFile;
  static thread_local std::string commonAncestor;
  static thread_local size_t obufferSize = 0;

  obuffer.reserve(PACK_BLOCK_SIZE);

  // 获取公共祖先
  if (commonAncestor.empty() && !flush) {
    commonAncestor = getCommonAncestor(files);
  }

  for (; currentFileIndex < files.size() && !flush; ++currentFileIndex) {
    fs::path filePath = fs::relative(files[currentFileIndex], commonAncestor);

    // 打开文件
    if (!inFile.is_open()) {
      inFile.open(files[currentFileIndex], std::ios::binary);
      if (!inFile) {
        throw std::runtime_error("Failed to open: " + filePath.string());
      }
    }

    // 创造header
    if (inFile.tellg() == 0) {
      // 计算文件大小
      FileDetail fd = getFileDetail(files[currentFileIndex]);
      size_t dataSize = fd.size;
      obufferSize += insertHeader(filePath, dataSize, obuffer, obufferSize);
    }

    // 读取文件数据并写入缓冲区
    while (true) {
      // 计算obuffer还能读入多少数据
      size_t remainingSpace = obuffer.capacity() - obufferSize;

      // 先扩大obuffer
      obuffer.resize(obuffer.capacity());

      // 尽量填满obuffer
      inFile.read(
        reinterpret_cast<char*>(obuffer.data() + obufferSize),
        static_cast<std::streamsize>(remainingSpace)
      );

      // 计算实际的读入数
      auto bytesRead = static_cast<size_t>(inFile.gcount());
      obufferSize += bytesRead;

      // 调整为实际的读入数
      obuffer.resize(obufferSize);

      // 如果缓冲区满，则返回
      if (obufferSize >= obuffer.capacity()) {
        obufferSize = 0;  // 清空标记
        return {true, obuffer};
      }

      // 如果文件读取完毕，并且obuffer还有空余，退出循环
      if (bytesRead < remainingSpace) {
        inFile.close();
        // 关闭inFile，此时会回到for循环，打开下一个文件的输入流
        break;
      }
    }
  }

  // 没有文件可读
  // 如果flush为true，输出剩余的缓冲区内容
  if (flush && obufferSize > 0) {
    obuffer.resize(obufferSize);
    obufferSize = 0;  // 清空缓冲区
    return {true, obuffer};
  }

  // 重置状态，等待下次复用
  currentFileIndex = 0;
  commonAncestor.clear();

  // 没有文件可读，flush也为false，那么返回false
  return {false, obuffer};
}  // namespace zipfiles::server

}  // namespace zipfiles::server
