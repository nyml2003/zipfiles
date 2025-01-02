#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>
#include "server/backup/backup.h"
#include "server/pack/pack.h"
#include "server/pack/unpack.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

fs::path test_files = "/app/test_files";  // NOLINT
fs::path pack_path = "/tmp/pack";         // NOLINT

class PackAndUnpackSingle : public ::testing::Test {
 protected:
  void SetUp() override { fs::remove_all(pack_path); }

  void TearDown() override { fs::remove_all(pack_path); }
};

TEST_F(PackAndUnpackSingle, PackAndUnpackSingle) {  // NOLINT
  // 读取文件
  std::vector<fs::path> pack_files{};
  pack_files.push_back(test_files / "text/mega_text_test_file");

  // 获取lca
  fs::path lca = getCommonAncestor(pack_files);

  // 初始化输出
  fs::create_directories(pack_path);
  fs::path opath(pack_path / "pack_file");
  std::ofstream ofile;
  ofile.open(opath, std::ios::binary);

  if (!ofile) {
    std::cout << "Failed to open: " << pack_path / "pack_file" << std::endl;
    return;
  }

  // 初始化flush条件
  bool flush = false;

  // Start timing for pack
  auto start_pack = std::chrono::high_resolution_clock::now();

  // pack
  while (true) {
    auto [pack_flush, packed_data] = packFilesByBlock(pack_files, flush, lca);

    if (pack_flush) {
      ofile.write(
        reinterpret_cast<const char*>(packed_data.data()),
        static_cast<std::streamsize>(packed_data.size())
      );
    }
    if (flush) {
      break;
    }
    if (!pack_flush && !packed_data.empty()) {
      flush = true;
    }
  }

  ofile.close();

  auto end_pack = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> pack_time = end_pack - start_pack;
  std::cout << "Pack time: " << pack_time.count() << " ms" << std::endl;

  // 计算总大小，获得Pack速度(MB/s)
  std::ifstream ifile_size;
  size_t total_size = 0;
  for (const auto& pack_file : pack_files) {
    ifile_size.open(pack_file, std::ios::binary);
    ifile_size.seekg(0, std::ios::end);
    total_size += ifile_size.tellg();
    ifile_size.close();
  }

  double pack_speed =
    static_cast<double>(total_size) / pack_time.count() * 1000 / 1024 / 1024;
  std::cout << "Pack speed: " << pack_speed << " MB/s" << std::endl;

  // unpack
  std::ifstream ifile;
  ifile.open(pack_path / "pack_file", std::ios::binary);

  auto start_unpack = std::chrono::high_resolution_clock::now();

  FileUnpacker unpacker(pack_path);

  try {
    std::vector<uint8_t> buffer(UNPACK_BLOCK_SIZE);
    while (ifile.read(
             reinterpret_cast<char*>(buffer.data()),
             static_cast<std::streamsize>(buffer.size())
           ) ||
           ifile.gcount() >= 0) {
      size_t bytesRead = ifile.gcount();
      // 更改buffer size为实际读取的大小
      buffer.resize(bytesRead);

      bool flush = false;
      if (buffer.size() < buffer.capacity()) {
        flush = true;
      }

      unpacker.unpackFilesByBlock(buffer, false);
      if (flush) {
        break;
      }
      buffer.resize(UNPACK_BLOCK_SIZE);
    }

    unpacker.unpackFilesByBlock(buffer, true);
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }

  auto end_unpack = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> unpack_time =
    end_unpack - start_unpack;
  std::cout << "Unpack time: " << unpack_time.count() << " ms" << std::endl;

  double unpack_speed =
    static_cast<double>(total_size) / unpack_time.count() * 1000 / 1024 / 1024;
  std::cout << "Unpack speed: " << unpack_speed << " MB/s" << std::endl;

  bool is_correct = true;

  // compare
  for (const auto& pack_file : pack_files) {
    fs::path filePath = pack_path / fs::relative(pack_file.parent_path(), lca);
    filePath = filePath / pack_file.filename();

    std::string cmpCommand =
      "cmp --silent " + pack_file.string() + " " + filePath.string();  // NOLINT
    ASSERT_EQ(system(cmpCommand.c_str()), 0) <<                        // NOLINT
      "文件 " << pack_file.string() << " 和 " << filePath.string() << " 不同";
    break;
  }
}

}  // namespace zipfiles::server