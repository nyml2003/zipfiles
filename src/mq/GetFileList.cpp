#include "mq/GetFileList.h"
#include <cstring>

namespace zipfiles::mq::GetFileList {

GetFileList::GetFileList(const std::vector<std::string>& filenames) {
  count = filenames.size();
  for (size_t i = 0; i < count; ++i) {
    filenameLengths.push_back(filenames[i].size());
    this->filenames.push_back(filenames[i]);
  }
}

void GetFileList::serialize(char* buffer, size_t& buffer_size) {
  size_t type_count = sizeof(count);
  memcpy(buffer + buffer_size, &count, type_count);
  buffer_size += type_count;
  for (size_t i = 0; i < count; ++i) {
    size_t len = strlen(filenames[i].c_str());
    size_t type_filenameLength = sizeof(filenameLength);
    size_t type_filename = sizeof(filename);
    memcpy(buffer + buffer_size, &len, type_filenameLength);
    buffer_size += type_filenameLength;
    memcpy(buffer + buffer_size, filenames[i].c_str(), type_filename * len);
    buffer_size += len;
  }
}

void GetFileList::deserialize(const char* buffer, size_t& offset) {
  size_t type_count = sizeof(count);
  memcpy(&count, buffer + offset, type_count);
  offset += type_count;

  for (size_t i = 0; i < count; ++i) {
    size_t len;
    size_t type_filenameLength = sizeof(filenameLength);
    size_t type_filename = sizeof(filename);
    memcpy(&len, buffer + offset, type_filenameLength);
    offset += type_filenameLength;
    filenameLengths.push_back(len);
    filenames.push_back(std::string(buffer + offset, len));
    offset += len;
  }
}

std::string GetFileList::to_string() {
  std::string result = "FileList: {";
  result += "count: " + std::to_string(count) + ", ";
  result += "filenames: [";
  for (size_t i = 0; i < count; ++i) {
    result += filenames[i];
    if (i != count - 1) {
      result += ", ";
    }
  }
  result += "]}";
  return result;
}

}  // namespace zipfiles::mq::GetFileList