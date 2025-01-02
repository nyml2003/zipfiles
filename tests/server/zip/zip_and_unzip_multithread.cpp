#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <vector>

#include "server/deflate/zip.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

std::string zip_path = "/tmp/zip";           // NOLINT
std::string test_files = "/app/test_files";  // NOLINT

bool test(const std::string& src_filename) {
  std::string dst_filename = src_filename + ".zip";
  std::string src_filename_ex = src_filename + ".unzip";

  // zip
  std::ifstream ifile;
  std::ofstream ofile;
  auto iflag = std::ios::in | std::ios::binary;
  auto oflag = std::ios::out | std::ios::binary | std::ios::trunc;
  fs::path ipath(test_files + src_filename);
  fs::path opath(zip_path + dst_filename);
  fs::create_directories(opath.parent_path());

  ifile.open(ipath, iflag);
  ofile.open(opath, oflag);

  if (!ifile) {
    std::cout << "Cant open file: " << ipath << std::endl;
    return false;
  }
  if (!ofile) {
    std::cout << "Cant open file: " << opath << std::endl;
    return false;
  }
  const int ibuf_size = (1143 << 16) + 191981;
  std::vector<std::uint8_t> ibuffer(ibuf_size);
  Zip zip_c;
  zip_c.init_worker();

  // Start timing for zip
  auto start_zip = std::chrono::high_resolution_clock::now();

  while (!ifile.eof()) {
    ifile.read(reinterpret_cast<char*>(ibuffer.data()), ibuf_size);

    ibuffer.resize(ifile.gcount());

    zip_c.fill_input(ibuffer, ifile.eof());

    while (zip_c.full() || ifile.eof()) {
      ZippedDataPacket zipped_data = zip_c.get_output(true);
      while (zipped_data.valid) {
        ofile.write(
          reinterpret_cast<char*>(zipped_data.data.data()),
          static_cast<int>(zipped_data.data.size())
        );
        if (zipped_data.eof) {
          break;
        }
        zipped_data = zip_c.get_output(ifile.eof());
      }
      if (zipped_data.eof) {
        break;
      }
    }
  }
  ifile.close();
  ofile.close();

  // End timing for zip and output the time taken
  auto end_zip = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> zip_time = end_zip - start_zip;
  std::cout << "Zip time: " << zip_time.count() << " ms" << std::endl;

  // 计算总大小，获得Zip速度(MB/s)
  std::ifstream ifile_size;
  size_t total_size = 0;
  ifile_size.open(ipath, std::ios::binary);
  ifile_size.seekg(0, std::ios::end);
  total_size += ifile_size.tellg();
  ifile_size.close();

  double zip_speed =
    static_cast<double>(total_size) / zip_time.count() * 1000 / 1024 / 1024;
  std::cout << "Zip speed: " << zip_speed << " MB/s" << std::endl;

  // unzip
  ifile.open(zip_path + dst_filename, iflag);
  ofile.open(zip_path + src_filename_ex, oflag);
  ibuffer.resize(ibuf_size);
  Unzip unzip_c;

  // Start timing for unzip
  auto start_unzip = std::chrono::high_resolution_clock::now();

  while (!ifile.eof()) {
    ifile.read(reinterpret_cast<char*>(ibuffer.data()), ibuf_size);
    ibuffer.resize(ifile.gcount());
    ZipStatus unzip_ret = {false, false, nullptr};
    unzip_c.reset_input(&ibuffer);
    while (!unzip_ret.lack) {
      // unzip_ret = unzip(ibuffer);
      unzip_ret = unzip_c.run();
      if (unzip_ret.flush) {
        ofile.write(
          reinterpret_cast<char*>(unzip_ret.obuffer->data()),
          static_cast<int>(unzip_ret.obuffer->size())
        );
      }
    }
  }
  ifile.close();
  ofile.close();

  // End timing for unzip and output the time taken
  auto end_unzip = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> unzip_time =
    end_unzip - start_unzip;
  std::cout << "Unzip time: " << unzip_time.count() << " ms" << std::endl;

  double unzip_speed =
    static_cast<double>(total_size) / unzip_time.count() * 1000 / 1024 / 1024;
  std::cout << "Unzip speed: " << unzip_speed << " MB/s" << std::endl;

  // compare
  std::string cmpCommand = "cmp --silent " + test_files + src_filename + " " +
                           zip_path + src_filename_ex;  // NOLINT
  if (system(cmpCommand.c_str()) != 0) {                // NOLINT
    std::cout << "文件 " << test_files + src_filename << " 和 "
              << zip_path + src_filename_ex << " 不同" << std::endl;
    return false;
  }
  return true;
}

class ZipAndUnzipMultiThread : public ::testing::Test {
 protected:
  void SetUp() override { fs::remove_all(zip_path); }

  void TearDown() override { fs::remove_all(zip_path); }
};

TEST_F(ZipAndUnzipMultiThread, SmallFile) {  // NOLINT
  ASSERT_TRUE(test("/text/small_text_test_file"));
}

TEST_F(ZipAndUnzipMultiThread, BigFile) {  // NOLINT
  ASSERT_TRUE(test("/text/big_text_test_file"));
}

TEST_F(ZipAndUnzipMultiThread, VeryBigFile) {  // NOLINT
  ASSERT_TRUE(test("/text/mega_text_test_file"));
}
}  // namespace zipfiles::server
