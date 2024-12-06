#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include "server/configure/configure.h"
#include "server/pack/pack.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {
/**
 * @brief 将FileDetail实例序列化并插入header
 *
 * @param fileDetail FileDetail实例
 *
 * @param header header
 *
 * @param structSize fileDetail的大小
 *
 */
void fileDetailSerialize(
  const FileDetail& fileDetail,
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
  std::memcpy(
    header.data() + offset, &fileDetail.type, sizeof(fileDetail.type)
  );
  offset += sizeof(fileDetail.type);

  // 写入创建时间
  std::memcpy(
    header.data() + offset, &fileDetail.createTime,
    sizeof(fileDetail.createTime)
  );
  offset += sizeof(fileDetail.createTime);

  // 写入修改时间
  std::memcpy(
    header.data() + offset, &fileDetail.updateTime,
    sizeof(fileDetail.updateTime)
  );
  offset += sizeof(fileDetail.updateTime);

  // 写入文件大小
  std::memcpy(
    header.data() + offset, &fileDetail.size, sizeof(fileDetail.size)
  );
  offset += sizeof(fileDetail.size);

  // 写入文件权限
  std::memcpy(
    header.data() + offset, &fileDetail.mode, sizeof(fileDetail.mode)
  );
  offset += sizeof(fileDetail.mode);

  // 写入拥有者
  size_t ownerSize = fileDetail.owner.size();
  std::memcpy(header.data() + offset, &ownerSize, sizeof(ownerSize));
  offset += sizeof(ownerSize);
  std::memcpy(header.data() + offset, fileDetail.owner.c_str(), ownerSize);
  offset += ownerSize;

  // 写入用户组
  size_t groupSize = fileDetail.group.size();
  std::memcpy(header.data() + offset, &groupSize, sizeof(groupSize));
  offset += sizeof(groupSize);
  std::memcpy(header.data() + offset, fileDetail.group.c_str(), groupSize);
  offset += groupSize;

  // 写入文件名
  size_t absolutePathSize = fileDetail.absolutePath.size();
  std::memcpy(
    header.data() + offset, &absolutePathSize, sizeof(absolutePathSize)
  );
  offset += sizeof(absolutePathSize);
  std::memcpy(
    header.data() + offset, fileDetail.absolutePath.c_str(), absolutePathSize
  );
  offset += absolutePathSize;

  // 写入设备号
  std::memcpy(header.data() + offset, &fileDetail.dev, sizeof(fileDetail.dev));
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
  const FileDetail& fileDetail,
  std::vector<uint8_t>& header
) {
  size_t relativePathSize = filePath.string().size();
  size_t structSize = sizeof(fileDetail.type) + sizeof(fileDetail.createTime) +
                      sizeof(fileDetail.updateTime) + sizeof(fileDetail.size) +
                      sizeof(fileDetail.mode) + sizeof(size_t) * 3 +
                      fileDetail.owner.size() + fileDetail.group.size() +
                      fileDetail.absolutePath.size() + sizeof(fileDetail.dev);
  size_t totalSize = sizeof(size_t) * 2 + relativePathSize + structSize;

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
  fileDetailSerialize(fileDetail, header, structSize);
}

/**
 * @brief
 * 传入文件路径数组，返回一个打包后的数据块(第一个块会带有额外信息以及部分文件数据)
 *
 * @param files 一个文件路径数组
 *
 * @param flush 是否强制输出缓冲区
 *
 */
std::pair<bool, std::vector<uint8_t>&> packFilesByBlock(
  const std::vector<fs::path>& files,
  bool flush,
  const fs::path& lca
) {
  static thread_local std::vector<uint8_t> obuffer;  // 默认为1<<19
  static thread_local size_t currentFileIndex = 0;
  static thread_local int inFile = -1;
  static thread_local std::string commonAncestor;
  static thread_local size_t obuffer_offset = 0;
  static thread_local std::vector<uint8_t> header_buffer;
  static thread_local size_t header_offset = 0;
  static thread_local FileDetail fileDetail;

  obuffer.reserve(PACK_BLOCK_SIZE);

  for (; currentFileIndex < files.size() && !flush; ++currentFileIndex) {
    fs::path filePath =
      fs::relative(files[currentFileIndex].parent_path(), lca);
    filePath = filePath / files[currentFileIndex].filename();

    // 获取文件信息
    if (inFile == -1) {
      fileDetail = getFileDetail(files[currentFileIndex]);
    }

    if (fileDetail.type == fs::file_type::socket) {
      // 不备份socket
      continue;
    }

    bool isRegular = (fileDetail.type == fs::file_type::regular);

    try {
      // 第一次打开普通文件
      if (inFile == -1 && isRegular) {
        inFile = open(files[currentFileIndex].c_str(), O_RDONLY);  // NOLINT
        if (!inFile) {
          throw std::runtime_error(
            "Failed to open: " + files[currentFileIndex].string()
          );
        }

        flock fl = {
          F_RDLCK,   // 设置锁类型为读锁
          SEEK_SET,  // 设置锁的起始位置为文件开头
          0,         // 锁的起始位置偏移量
          0,         // 锁的长度，0 表示到文件末尾
          getpid()};

        // 使用 fcntl 尝试在文件上加读锁
        // F_SETLKW 会阻塞直到锁可用
        if (fcntl(inFile, F_SETLKW, &fl) == -1) {  // NOLINT
          // 若失败，则关闭fd
          close(inFile);
          throw std::runtime_error(
            "Failed to acquire lock: " + files[currentFileIndex].string()
          );
        }
      }

      // 创造header
      if (lseek(inFile, 0, SEEK_CUR) == 0 || !isRegular) {
        if (header_buffer.empty()) {
          createHeader(filePath, fileDetail, header_buffer);
          if (fileDetail.type == fs::file_type::symlink) {
            // 对于软链接，直接再插入其指向的链接作为数据
            std::string target = fs::read_symlink(files[currentFileIndex]);
            header_buffer.insert(
              header_buffer.end(), target.begin(), target.end()
            );
          }
        }

        // 计算要往obuffer拷贝多少数据
        size_t to_copy =
          std::min(obuffer.capacity() - obuffer_offset, header_buffer.size());

        obuffer.insert(
          obuffer.end(),
          header_buffer.begin() + static_cast<std::ptrdiff_t>(header_offset),
          header_buffer.begin() +
            static_cast<std::ptrdiff_t>(header_offset + to_copy)
        );

        header_offset += to_copy;
        obuffer_offset += to_copy;

        // 如果header_buffer已经全部拷贝完，则清空
        if (header_offset >= header_buffer.size()) {
          header_buffer.clear();
          header_offset = 0;
        }
      }

      if (!isRegular) {
        // 文件不是普通文件或者链接
        // 可能是软链接，设备文件、FIFO、Socket、目录
        // 不要从这些文件用fstream读取数据
        continue;
      }

      // 读取文件数据并写入缓冲区
      while (true) {
        // 计算obuffer还能读入多少数据
        size_t remainingSpace = obuffer.capacity() - obuffer_offset;

        // 先扩大obuffer
        obuffer.resize(obuffer.capacity());

        // 尽量填满obuffer，并计算实际的读入数
        ssize_t bytesRead =
          read(inFile, obuffer.data() + obuffer_offset, remainingSpace);
        obuffer_offset += bytesRead;

        // 调整为实际的读入数
        obuffer.resize(obuffer_offset);

        // 如果缓冲区满，则返回
        if (obuffer_offset >= obuffer.capacity()) {
          obuffer_offset = 0;  // 清空标记
          return {true, obuffer};
        }

        // 如果文件读取完毕，并且obuffer还有空余，退出循环
        if (bytesRead < static_cast<ssize_t>(remainingSpace)) {
          // 释放锁
          flock fl = {
            F_UNLCK,   // 设置锁类型为读锁
            SEEK_SET,  // 设置锁的起始位置为文件开头
            0,         // 锁的起始位置偏移量
            0,         // 锁的长度，0 表示到文件末尾
            getpid()};

          // 设置锁类型为解锁
          if (fcntl(inFile, F_SETLKW, &fl) == -1) {  // NOLINT
            // NOLINT
            throw std::runtime_error(
              "Failed to release lock: " + files[currentFileIndex].string()
            );
          }

          close(inFile);
          inFile = -1;

          // 关闭inFile，此时会回到for循环，打开下一个文件的输入流
          break;
        }
      }
    } catch (std::exception& e) {
      // 释放锁
      flock fl{};
      fl.l_type = F_UNLCK;                       // 设置锁类型为解锁
      if (fcntl(inFile, F_SETLKW, &fl) == -1) {  // NOLINT
        throw std::runtime_error(
          "Failed to release lock: " + files[currentFileIndex].string()
        );
      }
      close(inFile);
      inFile = -1;

      // 重置状态，等待下次复用
      currentFileIndex = 0;
      inFile = -1;
      commonAncestor.clear();

      throw std::runtime_error(e.what());
    }
  }  // for

  // 没有文件可读
  // 如果flush为true，输出剩余的缓冲区内容
  if (flush && obuffer_offset > 0) {
    obuffer.resize(obuffer_offset);
    obuffer_offset = 0;  // 清空缓冲区
    return {true, obuffer};
  }

  // 重置状态，等待下次复用
  currentFileIndex = 0;
  inFile = -1;
  commonAncestor.clear();

  // 没有文件可读，flush也为false，那么返回false
  return {false, obuffer};
}  // namespace zipfiles::server

}  // namespace zipfiles::server
