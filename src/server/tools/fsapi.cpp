#include "server/tools/fsapi.h"
#include "common.h"
#include <pwd.h>
#include <grp.h>
#include "utils.h"
#include <iostream>
#include <vector>
namespace zipfiles::server {
std::vector<File> getFilesList(const fs::path& directory) {
  const fs::path directory_path = "/" / directory;
  std::cout << "Listing files in " << directory_path << std::endl;
  if (!fs::exists(directory_path)) {
    throw std::runtime_error("Directory does not exist.");
  }
  if (!fs::is_directory(directory_path)) {
    throw std::runtime_error(directory_path.string() + " is not a directory.");
  }
  std::vector<File> files;
  for (const auto& entry : fs::directory_iterator(directory_path)) {
    File file;
    file.name = entry.path().filename().string();
    file.type = entry.symlink_status().type();
    std::cout << "Found " << file.name << std::endl;
    std::cout << "Type: " << zipfiles::toString(entry.symlink_status().type())
              << std::endl;
    files.push_back(file);
  }
  return files;
}

FileDetail getFileDetail(const fs::path& file) {
  const fs::path file_path = "/" / file;
  std::cout << "Getting file details for " << file_path << std::endl;
  if (!fs::exists(file_path)) {
    throw std::runtime_error("File does not exist.");
  }

  struct stat file_stat {};
  if (stat(file_path.c_str(), &file_stat) != 0) {
    throw std::runtime_error("Failed to get file details.");
  }
  struct passwd* pwd = getpwuid(file_stat.st_uid);
  struct group* grp = getgrgid(file_stat.st_gid);

  FileDetail file_detail = {
    .type = fs::status(file_path).type(),
    .createTime = toIso8601(file_stat.st_ctime),
    .updateTime = toIso8601(file_stat.st_mtime),
    .size = fs::file_size(file_path),
    .owner = pwd->pw_name,
    .group = grp->gr_name,
    .mode = file_stat.st_mode,
    .path = file_path.string(),
    .name = file_path.filename().string(),
  };

  return file_detail;
}
}  // namespace zipfiles::server