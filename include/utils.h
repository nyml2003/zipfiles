#ifndef ZIPFILES_UTILS_H
#define ZIPFILES_UTILS_H
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles {
std::string toString(const fs::file_type& type);
fs::file_type toFileType(const std::string& type);
std::tm toTm(uint64_t timestamp);
std::string toIso8601(const std::tm& tm);
std::tm toTm(const std::string& iso8601);
uint64_t toTimestamp(std::tm& tm);
}  // namespace zipfiles
#endif  // !ZIPFILES_UTILS_H
