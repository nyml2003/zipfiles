#include "server/pack/pack.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include "mp/dto.h"
#include "server/backup/backup.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {

/**
 * @brief 将FileDetail实例序列化并插入header
 *
 * @param fd FileDetail实例
 *
 * @param header header
 *
 * @param structSize fd的大小
 *
 */
void fileDetailSerialize(
  const FileDetail& fd,
  std::vector<uint8_t>& header,
  size_t structSize
) {
  size_t initialSize = header.size();

  header.resize(
    initialSize + sizeof(size_t) + structSize
  );  // 预留空间存储结构体大小和内容
  size_t offset = initialSize;

  // 写入结构体大小
  std::memcpy(header.data() + offset, &structSize, sizeof(size_t));
  offset += sizeof(size_t);

  // 写入结构体内容
  // 写入文件类型
  std::memcpy(header.data() + offset, &fd.type, sizeof(fd.type));
  offset += sizeof(fd.type);

  // 写入创建时间
  std::memcpy(header.data() + offset, &fd.createTime, sizeof(fd.createTime));
  offset += sizeof(fd.createTime);

  // 写入修改时间
  std::memcpy(header.data() + offset, &fd.updateTime, sizeof(fd.updateTime));
  offset += sizeof(fd.updateTime);

  // 写入文件大小
  std::memcpy(header.data() + offset, &fd.size, sizeof(fd.size));
  offset += sizeof(fd.size);

  // 写入文件权限
  std::memcpy(header.data() + offset, &fd.mode, sizeof(fd.mode));
  offset += sizeof(fd.mode);

  // 写入拥有者
  size_t ownerSize = fd.owner.size();
  std::memcpy(header.data() + offset, &ownerSize, sizeof(ownerSize));
  offset += sizeof(ownerSize);
  std::memcpy(header.data() + offset, fd.owner.c_str(), ownerSize);
  offset += ownerSize;

  // 写入用户组
  size_t groupSize = fd.group.size();
  std::memcpy(header.data() + offset, &groupSize, sizeof(groupSize));
  offset += sizeof(groupSize);
  std::memcpy(header.data() + offset, fd.group.c_str(), groupSize);
  offset += groupSize;

  // 写入文件名
  size_t nameSize = fd.name.size();
  std::memcpy(header.data() + offset, &nameSize, sizeof(nameSize));
  offset += sizeof(nameSize);
  std::memcpy(header.data() + offset, fd.name.c_str(), nameSize);
  offset += nameSize;

  // 写入设备号
  std::memcpy(header.data() + offset, &fd.dev, sizeof(fd.dev));
}

/**
 * @brief 根据文件的路径以及数据大小，返回其header
 *
 * @param filePath 文件路径(相对)
 *
 * @param dataSize 文件大小
 *
 */
void createHeader(
  const fs::path& filePath,
  const FileDetail& fd,
  std::vector<uint8_t>& header
) {
  size_t relativePathSize = filePath.string().size();
  size_t structSize = sizeof(fd.type) + sizeof(fd.createTime) +
                      sizeof(fd.updateTime) + sizeof(fd.size) +
                      sizeof(fd.mode) + sizeof(size_t) * 3 + fd.owner.size() +
                      fd.group.size() + fd.name.size() + sizeof(fd.dev);
  size_t totalSize = sizeof(size_t) * 3 + relativePathSize + structSize;

  header.reserve(totalSize);

  // 写入文件路径大小
  header.insert(
    header.end(), reinterpret_cast<uint8_t*>(&relativePathSize),
    reinterpret_cast<uint8_t*>(&relativePathSize) + sizeof(size_t)
  );

  // 写入文件路径
  std::string filePathStr = filePath.string();
  header.insert(header.end(), filePathStr.begin(), filePathStr.end());

  // 写入FileDetail
  fileDetailSerialize(fd, header, structSize);

  // 写入文件数据大小
  size_t dataSize = fd.size;
  header.insert(
    header.end(), reinterpret_cast<uint8_t*>(&dataSize),
    reinterpret_cast<uint8_t*>(&dataSize) + sizeof(size_t)
  );
}

/**
 * @brief
 * 传入文件路径数组，返回一个打包后的数据块(第一个块会带有额外信息以及部分文件数据)
 *
 * @param files 一个文件路径数组
 *
 * @param flush 是否强制输出缓冲区
 */
std::pair<bool, std::vector<uint8_t>&>
packFilesByBlock(const std::vector<fs::path>& files, bool flush) {
  static thread_local std::vector<uint8_t> obuffer;  // 默认为1<<19
  static thread_local size_t currentFileIndex = 0;
  static thread_local std::ifstream inFile;
  static thread_local std::string commonAncestor;
  static thread_local size_t obufferSize = 0;
  static thread_local std::vector<uint8_t> header_buffer;
  static thread_local size_t header_offset = 0;

  obuffer.reserve(PACK_BLOCK_SIZE);

  // 获取公共祖先
  if (commonAncestor.empty() && !flush) {
    commonAncestor = getCommonAncestor(files);
  }

  for (; currentFileIndex < files.size() && !flush; ++currentFileIndex) {
    fs::path filePath = fs::relative(files[currentFileIndex], commonAncestor);

    // 获取文件信息
    FileDetail fd = getFileDetail(files[currentFileIndex]);

    if (fd.type == fs::file_type::socket) {
      // 不备份socket
      continue;
    }

    bool isRegular =
      (fd.type == fs::file_type::regular || fd.type == fs::file_type::symlink);

    // 打开文件
    if (!inFile.is_open() && isRegular) {
      inFile.open(files[currentFileIndex], std::ios::binary);
      if (!inFile) {
        throw std::runtime_error("Failed to open: " + filePath.string());
      }
    }

    // 创造header
    if (inFile.tellg() == 0 || !isRegular) {
      if (header_buffer.empty()) {
        createHeader(filePath, fd, header_buffer);
      }

      // 计算要往obuffer拷贝多少数据
      size_t to_copy =
        std::min(obuffer.capacity() - obufferSize, header_buffer.size());

      obuffer.insert(
        obuffer.end(),
        header_buffer.begin() + static_cast<std::ptrdiff_t>(header_offset),
        header_buffer.begin() +
          static_cast<std::ptrdiff_t>(header_offset + to_copy)
      );

      header_offset += to_copy;
      obufferSize += to_copy;

      // 如果header_buffer已经全部拷贝完，则清空
      if (header_offset >= header_buffer.size()) {
        header_buffer.clear();
        header_offset = 0;
      }
    }

    if (!isRegular) {
      // 文件不是普通文件或者链接
      // 可能是设备文件、FIFO、Socket、目录
      // 不要从这些文件读取数据，它们的大小是0
      continue;
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
