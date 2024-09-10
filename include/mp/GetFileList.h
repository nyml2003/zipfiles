#ifndef ZIPFILES_MQ_GET_FILE_LIST_H
#define ZIPFILES_MQ_GET_FILE_LIST_H
#include "mp/mp.h"
namespace zipfiles::mp {

class GetFileListResponse : public Jsonable {
 public:
  Json::Value toJson() override;
    void fromJson(const Json::Value& json) override;
    void setFilenames(std::vector<std::string> filenames);
    std::vector<std::string> getFilenames();

 private:
  std::vector<std::string> filenames;
};

}  // namespace zipfiles::mq::GetFileList
#endif  // !ZIPFILES_MQ_GET_FILE_LIST_H
