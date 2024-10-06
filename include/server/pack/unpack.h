#ifndef ZIPFILES_SERVER_UNPACK_H
#define ZIPFILES_SERVER_UNPACK_H

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles::server {

constexpr int UNPACK_BLOCK_SIZE = 1 << 21;

enum class State {
  READ_PATH_SIZE,
  READ_PATH,
  READ_DATA_SIZE,
  READ_DATA,
  FLUSH
};

/**
 * ! Deprecated
 *
 */
void unpackArchive(const fs::path& archivePath, const fs::path& outputDir);

void unpackFiles(const std::vector<uint8_t>& packedData, const fs::path& dst);

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
  size_t data_size{};
  size_t buffer_pos{};
  std::string file_path;
  std::ofstream output_file;
  std::vector<uint8_t> output_buffer;
  size_t output_buffer_pos{};
  std::vector<uint8_t> header_buffer;

  static constexpr size_t UNPACK_BLOCK_SIZE = 4096;

  void readPathSize(std::vector<uint8_t>& ibuffer);
  void readPath(std::vector<uint8_t>& ibuffer);
  void readDataSize(std::vector<uint8_t>& ibuffer);
  void readData(std::vector<uint8_t>& ibuffer);
  void flushBuffer();
};

}  // namespace zipfiles::server
   //
#endif  // !ZIPFILES_UNPACK_H