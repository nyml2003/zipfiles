#ifndef ZIPFILES_UTILS_H
#define ZIPFILES_UTILS_H
#include <filesystem>
#include <string>
namespace fs = std::filesystem;
namespace zipfiles {

std::string toString(const fs::file_type& type);
fs::file_type toFileType(const std::string& type);
double toDouble(const fs::file_type& type);
fs::file_type toFileType(const double& type);

}  // namespace zipfiles
#endif  // !ZIPFILES_UTILS_H
