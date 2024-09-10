#ifndef ZIPFILES_MQ_GET_FILE_LIST_H
#define ZIPFILES_MQ_GET_FILE_LIST_H
#include "mq/mq.h"
namespace zipfiles::mq::GetFileList {
typedef size_t count;
typedef size_t filenameLength;
typedef char filename;

class GetFileList : public IContent, public IToString {
 public:
  explicit GetFileList(const std::vector<std::string>& filenames);
  explicit GetFileList() = default;
  void serialize(char* buffer, size_t& buffer_size) override;
  void deserialize(const char* buffer, size_t& offset) override;
  std::string to_string() override;

 private:
  size_t count;
  std::vector<size_t> filenameLengths;
  std::vector<std::string> filenames;
};

}  // namespace zipfiles::mq::GetFileList
#endif  // !ZIPFILES_MQ_GET_FILE_LIST_H
