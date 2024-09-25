#include "utils.h"

namespace zipfiles {
std::string toString(const fs::file_type& type) {
  switch (type) {
    case fs::file_type::none:
      return "none";
    case fs::file_type::not_found:
      return "not_found";
    case fs::file_type::regular:
      return "regular";
    case fs::file_type::directory:
      return "directory";
    case fs::file_type::symlink:
      return "symlink";
    case fs::file_type::block:
      return "block";
    case fs::file_type::character:
      return "character";
    case fs::file_type::fifo:
      return "fifo";
    case fs::file_type::socket:
      return "socket";
    case fs::file_type::unknown:
      return "unknown";
    default:
      return "unknown";
  }

  return "unknown";
}

fs::file_type toFileType(const std::string& type) {
  if (type == "none") {
    return fs::file_type::none;
  } else if (type == "not_found") {
    return fs::file_type::not_found;
  } else if (type == "regular") {
    return fs::file_type::regular;
  } else if (type == "directory") {
    return fs::file_type::directory;
  } else if (type == "symlink") {
    return fs::file_type::symlink;
  } else if (type == "block") {
    return fs::file_type::block;
  } else if (type == "character") {
    return fs::file_type::character;
  } else if (type == "fifo") {
    return fs::file_type::fifo;
  } else if (type == "socket") {
    return fs::file_type::socket;
  } else if (type == "unknown") {
    return fs::file_type::unknown;
  } else {
    return fs::file_type::none;
  }
}

fs::file_type toFileType(const fs::directory_entry& entry) {
  return entry.symlink_status().type();
}

std::tm toTm(uint64_t timestamp) {
  std::time_t time = timestamp;
  return *std::localtime(&time);
}
std::string toIso8601(const std::tm& tm) {
  char buffer[80];
  std::strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S", &tm);
  return std::string(buffer);
}
std::tm toTm(const std::string& iso8601) {
  std::tm tm = {};
  std::istringstream ss(iso8601);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  return tm;
}
uint64_t toTimestamp(std::tm& tm) {
  return std::mktime(&tm);
}
}  // namespace zipfiles