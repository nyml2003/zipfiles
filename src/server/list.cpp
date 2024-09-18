#include "server/tools/fsapi.h"
#include "mp/GetFileList.h"
#include <iostream>
namespace zipfiles::server {
std::vector<File> list(const fs::path& source) {
  std::cout << "Listing files in " << source << std::endl;
  if (!fs::exists(source)) {
    throw std::runtime_error("Source does not exist.");
  }
  if (!fs::is_directory(source)) {
    throw std::runtime_error("Source is not a directory.");
  }
  std::vector<File> files;
  for (const auto& entry : fs::directory_iterator(source)) {
    File file;
    file.name = entry.path().filename().string();
    file.type =
      fs::is_directory(entry.path()) ? FileType::DIRECTORY : FileType::FILE;
    std::cout << "Found " << file.name << std::endl;
    std::cout << "Type: "
              << (file.type == FileType::FILE ? "file" : "directory")
              << std::endl;
    files.push_back(file);
  }
  return files;
}
}  // namespace zipfiles::server::fsapi