#include "utils.h"
#include <string>
#include <unordered_map>

namespace zipfiles {
std::string toString(const fs::file_type& type) {
  static const std::unordered_map<fs::file_type, std::string> fileTypeMap = {
    {fs::file_type::none, "none"},
    {fs::file_type::not_found, "not_found"},
    {fs::file_type::regular, "regular"},
    {fs::file_type::directory, "directory"},
    {fs::file_type::symlink, "symlink"},
    {fs::file_type::block, "block"},
    {fs::file_type::character, "character"},
    {fs::file_type::fifo, "fifo"},
    {fs::file_type::socket, "socket"},
    {fs::file_type::unknown, "unknown"}};

  auto it = fileTypeMap.find(type);
  if (it != fileTypeMap.end()) {
    return it->second;
  }
  throw std::invalid_argument("Unknown file type");
}

fs::file_type toFileType(const std::string& type) {
  static const std::unordered_map<std::string, fs::file_type> fileTypeMap = {
    {"none", fs::file_type::none},
    {"not_found", fs::file_type::not_found},
    {"regular", fs::file_type::regular},
    {"directory", fs::file_type::directory},
    {"symlink", fs::file_type::symlink},
    {"block", fs::file_type::block},
    {"character", fs::file_type::character},
    {"fifo", fs::file_type::fifo},
    {"socket", fs::file_type::socket},
    {"unknown", fs::file_type::unknown}};

  auto it = fileTypeMap.find(type);
  if (it != fileTypeMap.end()) {
    return it->second;
  }
  throw std::invalid_argument("Unknown file type");
}

fs::file_type toFileType(const fs::directory_entry& entry) {
  return entry.symlink_status().type();
}

std::string toIso8601(time_t timestamp) {
  std::tm tm = *std::localtime(&timestamp);
  std::string iso8601(20, '\0');
  if (std::strftime(iso8601.data(), iso8601.size(), "%Y-%m-%dT%H:%M:%S", &tm) == 0) {
    throw std::runtime_error("Failed to format timestamp to ISO 8601");
  }
  return iso8601;
}

std::string toIso8601(uint64_t timestamp) {
  return toIso8601(static_cast<time_t>(timestamp));
}
time_t toTimestamp(const std::string& iso8601) {
  std::tm tm = {};
  std::istringstream ss(iso8601);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  return std::mktime(&tm);
}
}  // namespace zipfiles