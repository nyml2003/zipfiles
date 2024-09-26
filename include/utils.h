#ifndef ZIPFILES_UTILS_H
#define ZIPFILES_UTILS_H
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles {

std::string toString(const fs::file_type& type);

fs::file_type toFileType(const std::string& type);

std::string toIso8601(time_t timestamp);

time_t toTimestamp(const std::string& iso8601);

std::string toIso8601(uint64_t timestamp);

}  // namespace zipfiles
#endif  // !ZIPFILES_UTILS_H
