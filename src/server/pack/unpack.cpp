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

/**
 * @brief 给定一个打包过后的数据流，恢复成多个文件
 *
 * @param ibuffer 输入缓冲区
 *
 * @param dst 要恢复到的目录
 *
 * @param flush 是否强制输出缓冲区
 *
 */
std::pair<bool, std::vector<uint8_t>&> unpackFilesByBlock(
  std::vector<uint8_t>& ibuffer,
  const fs::path& dst,
  bool flush
) {
  // 当前状态，初始化为READ_PATH_SIZE
  static thread_local State state = State::READ_PATH_SIZE;
  // 读出的path_size
  static thread_local size_t path_size = 0;
  // 读出的data_size
  static thread_local size_t data_size = 0;
  // 输入buffer的指针
  static thread_local size_t buffer_pos = 0;
  // 文件路径
  static thread_local std::string file_path;
  // 文件输出流
  static thread_local std::ofstream output_file;
  // 输出buffer
  static thread_local std::vector<uint8_t> output_buffer(UNPACK_BLOCK_SIZE);
  // 输出buffer的指针
  static thread_local size_t output_buffer_pos = 0;
  // 存储header信息的buffer
  static thread_local std::vector<uint8_t> header_buffer;

  while (true) {
    // buffer_pos大于ibuffer.size的情况
    if (buffer_pos > ibuffer.size()) {
      throw std::runtime_error("buffer_pos exceeds ibuffer.size()");
      return {false, output_buffer};
    }

    switch (state) {
      case State::READ_PATH_SIZE:
        if (header_buffer.size() < sizeof(size_t)) {
          // header_buffer数据不足时
          // 从ibuffer向header_buffer读入数据，读入大小是ibuffer的剩余数据量或者header_buffer的剩余数据量
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
          // header_buffer数据足够
          path_size = *reinterpret_cast<size_t*>(header_buffer.data());
          header_buffer.clear();
          // 跳转到READ_PATH
          state = State::READ_PATH;
        } else {
          if (flush) {
            state = State::FLUSH;
            break;
          }
          return {false, output_buffer};
        }
        break;

      case State::READ_PATH:
        if (header_buffer.size() < path_size) {
          // header_buffer数据不足时
          // 从ibuffer向header_buffer读入数据，读入大小是ibuffer的剩余数据量或者header_buffer的剩余数据量
          size_t to_copy = std::min(
            path_size - header_buffer.size(), ibuffer.size() - buffer_pos
          );
          header_buffer.insert(
            header_buffer.end(),
            ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
            ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos + to_copy)
          );
          buffer_pos += to_copy;
        }

        if (header_buffer.size() >= path_size) {
          // header_buffer数据足够
          file_path = std::string(header_buffer.begin(), header_buffer.end());
          header_buffer.clear();
          // 跳转到READ_DATA_SIZE
          state = State::READ_DATA_SIZE;
        } else {
          if (flush) {
            state = State::FLUSH;
            break;
          }
          return {false, output_buffer};
        }
        break;

      case State::READ_DATA_SIZE:
        if (header_buffer.size() < sizeof(size_t)) {
          // header_buffer数据不足时
          // 从ibuffer向header_buffer读入数据，读入大小是ibuffer的剩余数据量或者header_buffer的剩余数据量
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
          // header_buffer数据足够
          data_size = *reinterpret_cast<size_t*>(header_buffer.data());
          header_buffer.clear();
          // 跳转到READ_DATA
          state = State::READ_DATA;
        } else {
          if (flush) {
            state = State::FLUSH;
            break;
          }
          return {false, output_buffer};
        }
        break;

      case State::READ_DATA:
        while (data_size > 0) {
          // 因为可能ibuffer没读完，但obuffer已满，所以使用循环读入

          // 计算希望拷贝多少数据到obuffer
          // 为obuffer的剩余空间或剩余的全部数据
          size_t to_copy =
            std::min(data_size, UNPACK_BLOCK_SIZE - output_buffer_pos);

          if (ibuffer.size() - buffer_pos >= to_copy) {
            // 如果ibuffer有足够的数据
            std::copy(
              ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
              ibuffer.begin() +
                static_cast<std::ptrdiff_t>(buffer_pos + to_copy),
              output_buffer.begin() +
                static_cast<std::ptrdiff_t>(output_buffer_pos)
            );
            buffer_pos += to_copy;
            output_buffer_pos += to_copy;
            data_size -= to_copy;

            if (output_buffer_pos == UNPACK_BLOCK_SIZE) {
              // obuffer若读满
              if (!output_file.is_open()) {
                output_file.open(dst / file_path, std::ios::binary);
                if (!output_file) {
                  throw std::runtime_error("Failed to open file: " + file_path);
                }
              }
              // 写文件
              output_file.write(
                reinterpret_cast<char*>(output_buffer.data()),
                static_cast<std::streamsize>(output_buffer_pos)
              );
              output_buffer_pos = 0;
            }
          } else {
            // ibuffer没有足够的数据来填满obuffer或传递所有的文件数据
            // 计算ibuffer能拷贝出多少数据
            size_t remaining = ibuffer.size() - buffer_pos;
            // 拷贝
            std::copy(
              ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
              ibuffer.end(),
              output_buffer.begin() +
                static_cast<std::ptrdiff_t>(output_buffer_pos)
            );
            output_buffer_pos += remaining;
            data_size -= remaining;
            buffer_pos = 0;
            ibuffer.clear();
            return {false, output_buffer};
          }
        }

        if (data_size == 0) {
          // 如果已经读完全部的数据了
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
          // 并没有读完全部的数据，等待下一次ibuffer
          buffer_pos = 0;
          ibuffer.clear();
          return {false, output_buffer};
        }
        break;

      case State::FLUSH:
        // 强制输出obuffer的剩余内容
        if (output_file.is_open()) {
          output_file.write(
            reinterpret_cast<char*>(output_buffer.data()),
            static_cast<std::streamsize>(output_buffer_pos)
          );
          output_file.close();
        }
        ibuffer.clear();
        buffer_pos = 0;
        return {true, output_buffer};

      default:
        throw std::runtime_error("Unexpected state in unpackFilesByBlock");
        return {false, output_buffer};
    }
  }
}

}  // namespace zipfiles::server
