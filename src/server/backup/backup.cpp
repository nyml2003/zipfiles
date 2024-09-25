#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "server/backup/backup.h"

namespace zipfiles::server {

// 备份文件
void backup_file(const BackupParams& params) {
  std::ifstream ifs(params.source, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error(
      "Failed to open source file: " + params.source.string()
    );
  }
  std::ofstream ofs(params.destination, std::ios::binary);
  if (!ofs) {
    throw std::runtime_error(
      "Failed to open destination file: " + params.destination.string()
    );
  }
  ofs << ifs.rdbuf();
}

// 备份目录
void backup_directory(const BackupParams& params) {
  if (!fs::exists(params.source)) {
    throw std::runtime_error(
      "Source directory does not exist: " + params.source.string()
    );
  }
  fs::create_directories(params.destination);
  for (const auto& entry : fs::recursive_directory_iterator(params.source)) {
    const auto& path = entry.path();
    auto relative_path = fs::relative(path, params.source);
    if (fs::is_directory(path)) {
      fs::create_directories(params.destination / relative_path);
    } else if (fs::is_regular_file(path)) {
      backup_file(
        {.source = path, .destination = params.destination / relative_path}
      );
    } else {
      throw std::runtime_error(
        "Source path is neither a file nor a directory: " + path.string()
      );
    }
  }
}

// 备份
void backup(const BackupParams& params) {
  if (!fs::exists(params.source)) {
    throw std::runtime_error(
      "Source path does not exist: " + params.source.string()
    );
  }
  if (fs::is_directory(params.source)) {
    backup_directory(params);
  } else if (fs::is_regular_file(params.source)) {
    backup_file(params);
  } else {
    throw std::runtime_error(
      "Source path is neither a file nor a directory: " + params.source.string()
    );
  }
}

// 还原文件
void restore_file(const BackupParams& params) {
  std::ifstream ifs(params.source, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error(
      "Failed to open source file: " + params.source.string()
    );
  }
  std::ofstream ofs(params.destination, std::ios::binary);
  if (!ofs) {
    throw std::runtime_error(
      "Failed to open destination file: " + params.destination.string()
    );
  }
  ofs << ifs.rdbuf();
}

// 还原目录
void restore_directory(const BackupParams& params) {
  if (!fs::exists(params.source)) {
    throw std::runtime_error(
      "Source directory does not exist: " + params.source.string()
    );
  }
  fs::create_directories(params.destination);
  for (const auto& entry : fs::recursive_directory_iterator(params.source)) {
    const auto& path = entry.path();
    auto relative_path = fs::relative(path, params.source);
    if (fs::is_directory(path)) {
      fs::create_directories(params.destination / relative_path);
    } else if (fs::is_regular_file(path)) {
      restore_file(
        {.source = path, .destination = params.destination / relative_path}
      );
    } else {
      throw std::runtime_error(
        "Source path is neither a file nor a directory: " + path.string()
      );
    }
  }
}

// 还原
void restore(const BackupParams& params) {
  if (!fs::exists(params.source)) {
    throw std::runtime_error(
      "Source path does not exist: " + params.source.string()
    );
  }
  if (fs::is_directory(params.source)) {
    restore_directory(params);
  } else if (fs::is_regular_file(params.source)) {
    restore_file(params);
  } else {
    throw std::runtime_error(
      "Source path is neither a file nor a directory: " + params.source.string()
    );
  }
}

}  // namespace zipfiles::server