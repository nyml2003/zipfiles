#ifndef ZIPFILES_SERVER_UNPACK_H
#define ZIPFILES_SERVER_UNPACK_H

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>
#include "server/tools/fsapi.h"

namespace zipfiles::server {

constexpr int UNPACK_BLOCK_SIZE = 1 << 21;

enum class State {
  READ_PATH_SIZE,
  READ_PATH,
  READ_FILEDETAIL_SIZE,
  READ_FILEDETAIL,
  READ_DATA,
  FLUSH
};

/**
 * * unpack
 *
 */
class FileUnpacker {
 public:
  explicit FileUnpacker(fs::path destination) : dst(std::move(destination)) {
    output_buffer.resize(UNPACK_BLOCK_SIZE);
  }

  bool unpackFilesByBlock(std::vector<uint8_t>& ibuffer, bool flush);

 private:
  fs::path dst;
  State state{};
  size_t path_size{};
  size_t fileDetail_size{};
  size_t data_size{};
  size_t buffer_pos{};
  std::string file_path;
  FileDetail fd;
  std::ofstream output_file;
  std::vector<uint8_t> output_buffer;
  size_t output_buffer_pos{};
  std::vector<uint8_t> header_buffer;

  static void
  fileDetailDeserialize(FileDetail& fd, const std::vector<uint8_t>& header);
  void openOutputFileStream();
  void createSymlink(const std::string& target);
  void createDeviceFile();
  void createFIFO();

  void readPathSize(std::vector<uint8_t>& ibuffer);
  void readPath(std::vector<uint8_t>& ibuffer);
  void readFileDetailSize(std::vector<uint8_t>& ibuffer);
  void readFileDetail(std::vector<uint8_t>& ibuffer);
  void readData(std::vector<uint8_t>& ibuffer);
  void flushBuffer();
};

}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_UNPACK_H