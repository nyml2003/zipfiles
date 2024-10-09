#include "server/pack/unpack.h"
#include <grp.h>
#include <pwd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <utime.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace zipfiles::server {

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
        try {
          readPathSize(ibuffer);
        } catch (std::exception& e) {
          throw std::runtime_error(
            "Failed to read path size: " + std::string(e.what())
          );
        }
        break;

      case State::READ_PATH:
        try {
          readPath(ibuffer);
        } catch (std::exception& e) {
          throw std::runtime_error(
            "Failed to read path: " + std::string(e.what())
          );
        }
        break;

      case State::READ_FILEDETAIL_SIZE:
        try {
          readFileDetailSize(ibuffer);
        } catch (std::exception& e) {
          throw std::runtime_error(
            "Failed to read file detail size: " + std::string(e.what())
          );
        }
        break;

      case State::READ_FILEDETAIL:
        try {
          readFileDetail(ibuffer);
        } catch (std::exception& e) {
          throw std::runtime_error(
            "Failed to read file detail: " + std::string(e.what())
          );
        }
        break;

      case State::READ_DATA_SIZE:
        try {
          readDataSize(ibuffer);
        } catch (std::exception& e) {
          throw std::runtime_error(
            "Failed to read data size: " + std::string(e.what())
          );
        }
        break;

      case State::READ_DATA:
        try {
          readData(ibuffer);
        } catch (std::exception& e) {
          throw std::runtime_error(
            "Failed to read data: " + std::string(e.what())
          );
        }

        break;

      case State::FLUSH:
        try {
          flushBuffer();
        } catch (std::exception& e) {
          throw std::runtime_error("Failed to flush: " + std::string(e.what()));
        }
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

void FileUnpacker::fileDetailDeserialize(
  FileDetail& fd,
  const std::vector<uint8_t>& header
) {
  size_t offset = 0;

  // 读取结构体内容
  // 读取文件类型
  std::memcpy(&fd.type, header.data() + offset, sizeof(fd.type));
  offset += sizeof(fd.type);

  // 读取创建时间
  std::memcpy(&fd.createTime, header.data() + offset, sizeof(fd.createTime));
  offset += sizeof(fd.createTime);

  // 读取修改时间
  std::memcpy(&fd.updateTime, header.data() + offset, sizeof(fd.updateTime));
  offset += sizeof(fd.updateTime);

  // 读取大小
  std::memcpy(&fd.size, header.data() + offset, sizeof(fd.size));
  offset += sizeof(fd.size);

  // 读取权限
  std::memcpy(&fd.mode, header.data() + offset, sizeof(fd.mode));
  offset += sizeof(fd.mode);

  // 读取用户
  size_t ownerSize = 0;
  std::memcpy(&ownerSize, header.data() + offset, sizeof(ownerSize));
  offset += sizeof(ownerSize);
  fd.owner.resize(ownerSize);

  std::memcpy(fd.owner.data(), header.data() + offset, ownerSize);
  offset += ownerSize;

  // 读取用户组
  size_t groupSize = 0;
  std::memcpy(&groupSize, header.data() + offset, sizeof(groupSize));
  offset += sizeof(groupSize);
  fd.group.resize(groupSize);

  std::memcpy(fd.group.data(), header.data() + offset, groupSize);
  offset += groupSize;

  // 读取文件名
  size_t nameSize = 0;
  std::memcpy(&nameSize, header.data() + offset, sizeof(nameSize));
  offset += sizeof(nameSize);
  fd.name.resize(nameSize);

  std::memcpy(fd.name.data(), header.data() + offset, nameSize);
  offset += nameSize;

  // 读取设备号
  std::memcpy(&fd.dev, header.data() + offset, sizeof(fd.dev));
}

void FileUnpacker::openOutputFileStream() {
  // 打开相应的输出流
  if (!output_file.is_open()) {
    fs::path path = dst / file_path;
    fs::create_directories(path.parent_path());
    // 覆盖式写
    output_file.open(path, std::ios::binary | std::ios::trunc);
    if (!output_file) {
      throw std::runtime_error("Failed to open file: " + path.string());
    }
  }
}

void FileUnpacker::createDeviceFile() {
  // 无论是块设备还是字符设备，都可以处理
  fs::path path = (dst / file_path);
  fs::create_directories(path.parent_path());
  // 覆盖写
  fs::remove(path);
  if (mknod(path.c_str(), fd.mode, fd.dev) < 0) {
    throw std::runtime_error("Cannot create dev file: " + path.string());
  }
}

void FileUnpacker::createFIFO() {
  fs::path path = (dst / file_path);
  fs::create_directories(path.parent_path());
  // 覆盖写
  fs::remove(path);
  if (mkfifo(path.c_str(), fd.mode)) {
    throw std::runtime_error("Cannot create FIFO: " + path.string());
  }
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
    // 清空并转入下一状态
    header_buffer.clear();
    state = State::READ_FILEDETAIL_SIZE;
  }
}

void FileUnpacker::readFileDetailSize(std::vector<uint8_t>& ibuffer) {
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
    fileDetail_size = *reinterpret_cast<size_t*>(header_buffer.data());
    // 清空并转入下一状态
    header_buffer.clear();
    state = State::READ_FILEDETAIL;
  }
}

void FileUnpacker::readFileDetail(std::vector<uint8_t>& ibuffer) {
  if (header_buffer.size() < fileDetail_size) {
    // 若header_buffer没有读够预期的内容
    // 从ibuffer读满header_buffer，或ibuffer数据不足，则读取ibuffer剩下的全部内容
    size_t to_copy = std::min(
      fileDetail_size - header_buffer.size(), ibuffer.size() - buffer_pos
    );
    header_buffer.insert(
      header_buffer.end(),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos),
      ibuffer.begin() + static_cast<std::ptrdiff_t>(buffer_pos + to_copy)
    );
    buffer_pos += to_copy;
  }

  if (header_buffer.size() >= fileDetail_size) {
    // header_buffer内容足够
    fileDetailDeserialize(fd, header_buffer);

    // 根据文件类型处理
    switch (fd.type) {
      case std::filesystem::file_type::none:
        throw std::runtime_error("File type is none, file: " + file_path);
        break;
      case std::filesystem::file_type::not_found:
        throw std::runtime_error("File type is not_found, file: " + file_path);
        break;
      case std::filesystem::file_type::regular:
        openOutputFileStream();
        break;
      case std::filesystem::file_type::directory:
        fs::create_directories(dst / file_path);
        break;
      case std::filesystem::file_type::symlink:
        openOutputFileStream();
        break;
      case std::filesystem::file_type::character:
        createDeviceFile();
        break;
      case std::filesystem::file_type::fifo:
        createFIFO();
        break;
      case std::filesystem::file_type::socket:
        throw std::runtime_error("Socket file");
        break;
      case std::filesystem::file_type::block:
        createDeviceFile();
        break;
      case std::filesystem::file_type::unknown:
        break;
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

    // 收尾时恢复文件信息
    // 设置文件修改时间
    std::string path = dst / file_path;
    struct utimbuf new_times {};
    new_times.actime = time(nullptr);  // 保持当前访问时间不变
    new_times.modtime = static_cast<time_t>(fd.updateTime);
    if (utime(path.c_str(), &new_times) != 0) {
      throw std::runtime_error("Cannot set time correctly, file: " + path);
    }

    // 设置文件所有者和组
    struct passwd* pwd = getpwnam(fd.owner.c_str());
    struct group* grp = getgrnam(fd.group.c_str());
    if (pwd != nullptr && grp != nullptr) {
      if (chown(path.c_str(), pwd->pw_uid, grp->gr_gid) != 0) {
        throw std::runtime_error(
          "Cannot set owner and group correctly, file: " + path
        );
      }
    } else {
      throw std::runtime_error("Invalid user or group, file: " + path);
    }

    // 设置文件权限
    if (chmod(path.c_str(), fd.mode) != 0) {
      throw std::runtime_error("Cannot set mode correctly, file: " + path);
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
