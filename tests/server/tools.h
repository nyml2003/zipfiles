#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

inline void get_test_files(const fs::path& directory, std::vector<fs::path>& files) {
  try {
    // 检查路径是否存在
    if (fs::exists(directory) && fs::is_directory(directory)) {
      // 遍历目录
      for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        // 检查是否不是目录
        if (!fs::is_directory(entry)) {
          files.push_back(entry.path());  // 添加文件的绝对路径
        }
      }
    }
  } catch (const fs::filesystem_error& e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "General error: " << e.what() << std::endl;
  }
}

inline uintmax_t get_folder_size(const fs::path& path) {
  uintmax_t size = 0;

  // 检查路径是否存在且为目录
  if (fs::exists(path) && fs::is_directory(path)) {
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
      if (fs::is_regular_file(entry)) {
        // 累加文件大小
        size += fs::file_size(entry);
      }
    }
  }

  return size;
}