#include "server/pack/unpack.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

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
    // archive.read(pathStr.data(), pathLength);

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
      // archive.read(buffer.data(), readSize);
      // file.write(buffer.data(), readSize);
      remaining_size -= readSize;
    }
  }
}

std::string stateToString(State state) {
  switch (state) {
    case State::READ_PATH_SIZE:
      return "READ_PATH_SIZE";
    case State::READ_PATH:
      return "READ_PATH";
    case State::READ_DATA_SIZE:
      return "READ_DATA_SIZE";
    case State::READ_DATA:
      return "READ_DATA";
    case State::FLUSH:
      return "FLUSH";
    default:
      return "UNKNOWN";
  }
}

bool FileUnpacker::unpackFilesByBlock(
  std::vector<uint8_t>& ibuffer,
  bool flush
) {
  while (buffer_pos < ibuffer.size()) {
    if (buffer_pos >= ibuffer.size()) {
      throw std::runtime_error("buffer_pos exceeds ibuffer.size()");
      return false;
    }

    switch (state) {
      case State::READ_PATH_SIZE:
        readPathSize(ibuffer);
        break;

      case State::READ_PATH:
        readPath(ibuffer);
        break;

      case State::READ_DATA_SIZE:
        readDataSize(ibuffer);
        break;

      case State::READ_DATA:
        readData(ibuffer);
        break;

      case State::FLUSH:
        flushBuffer();
        return true;

      default:
        throw std::runtime_error("Unexpected state in unpackFilesByBlock");
        return false;
    }

    if (flush && state != State::FLUSH) {
      state = State::FLUSH;
    }

    if (state == State::FLUSH) {
      break;
    }
  }

  // ibuffer已经没有东西可以读了，则返回false，在外部循环检查是否有下一个ibuffer
  return false;
}

void FileUnpacker::readPathSize(std::vector<uint8_t>& ibuffer) {
  if (header_buffer.size() < sizeof(size_t)) {
    // 若header_buffer没有读够预期的内容
    // 从ibuffer读满header_buffer，或ibuffer数据不足，则读取ibuffer剩下的全部内容

    size_t to_copy = std::min(
      sizeof(size_t) - header_buffer.size(), ibuffer.size() - buffer_pos
    );
    header_buffer.insert(
      header_buffer.end(),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos + to_copy)
    );
    buffer_pos += to_copy;
  }

  if (header_buffer.size() >= sizeof(size_t)) {
    // header_buffer内容足够
    path_size = *reinterpret_cast<size_t*>(header_buffer.data());
    // 清空并转入下一状态
    header_buffer.clear();
    state = State::READ_PATH;
  }
}

void FileUnpacker::readPath(std::vector<uint8_t>& ibuffer) {
  if (header_buffer.size() < path_size) {
    // 若header_buffer没有读够预期的内容
    // 从ibuffer读满header_buffer，或ibuffer数据不足，则读取ibuffer剩下的全部内容
    size_t to_copy =
      std::min(path_size - header_buffer.size(), ibuffer.size() - buffer_pos);
    header_buffer.insert(
      header_buffer.end(),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos + to_copy)
    );
    buffer_pos += to_copy;
  }

  if (header_buffer.size() >= path_size) {
    // header_buffer内容足够
    file_path = std::string(header_buffer.begin(), header_buffer.end());

    // 打开相应的输出流
    if (!output_file.is_open()) {
      fs::path full_path = dst / file_path;
      fs::create_directories(full_path.parent_path());
      output_file.open(full_path, std::ios::binary);
      if (!output_file) {
        throw std::runtime_error("Failed to open file: " + full_path.string());
      }
    }

    // 清空并转入下一状态
    header_buffer.clear();
    state = State::READ_DATA_SIZE;
  }
}

void FileUnpacker::readDataSize(std::vector<uint8_t>& ibuffer) {
  if (header_buffer.size() < sizeof(size_t)) {
    // 若header_buffer没有读够预期的内容
    // 从ibuffer读满header_buffer，或ibuffer数据不足，则读取ibuffer剩下的全部内容
    size_t to_copy = std::min(
      sizeof(size_t) - header_buffer.size(), ibuffer.size() - buffer_pos
    );
    header_buffer.insert(
      header_buffer.end(),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos + to_copy)
    );
    buffer_pos += to_copy;
  }

  if (header_buffer.size() >= sizeof(size_t)) {
    // header_buffer内容足够
    data_size = *reinterpret_cast<size_t*>(header_buffer.data());
    // 清空并转入下一状态
    header_buffer.clear();
    state = State::READ_DATA;
  }
}

void FileUnpacker::readData(std::vector<uint8_t>& ibuffer) {
  while (data_size > 0) {
    // 有可能obuffer满，但ibuffer的数据还没读完，或文件数据还不完整
    // 使用循环来确保obuffer能读完整个文件

    // 确定这次读入是填满obuffer，还是能够读取完所有数据
    size_t to_copy = std::min(data_size, UNPACK_BLOCK_SIZE - output_buffer_pos);

    if (ibuffer.size() - buffer_pos >= to_copy) {
      // 如果ibuffer的数据足够填满obuffer，或者足够完成一次文件输出
      std::copy(
        ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
        ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos + to_copy),
        output_buffer.begin() + static_cast<std::ptrdiff_t>(output_buffer_pos)
      );
      buffer_pos += to_copy;
      output_buffer_pos += to_copy;
      data_size -= to_copy;

      if (output_buffer_pos == UNPACK_BLOCK_SIZE) {
        // obuffer满，则先输出到文件
        output_file.write(
          reinterpret_cast<char*>(output_buffer.data()),
          static_cast<std::streamsize>(output_buffer_pos)
        );
        output_buffer_pos = 0;
      }
    } else {
      // ibuffer的数据不足以填满obuffer或完成一次输出
      // 先拷贝ibuffer的剩余数据，等待下一次ibuffer
      size_t remaining = ibuffer.size() - buffer_pos;
      std::copy(
        ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
        ibuffer.end(),
        output_buffer.begin() + static_cast<std::ptrdiff_t>(output_buffer_pos)
      );
      output_buffer_pos += remaining;
      data_size -= remaining;
      buffer_pos = 0;
      ibuffer.clear();
      return;
    }
  }

  if (data_size == 0) {
    // 如果当前文件已完成
    if (output_file.is_open()) {
      output_file.write(
        reinterpret_cast<char*>(output_buffer.data()),
        static_cast<std::streamsize>(output_buffer_pos)
      );
      output_file.close();
      output_buffer_pos = 0;
    }

    state = State::READ_PATH_SIZE;
  } else {
    // 没有完成，则也清空ibuffer
    buffer_pos = 0;
    ibuffer.clear();
  }
}

void FileUnpacker::flushBuffer() {
  if (output_file.is_open()) {
    output_file.write(
      reinterpret_cast<char*>(output_buffer.data()),
      static_cast<std::streamsize>(output_buffer_pos)
    );
    output_file.close();
  }
  buffer_pos = 0;
}

}  // namespace zipfiles::server
