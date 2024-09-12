#ifndef ZIPFILES_MQ_GET_FILE_LIST_H
#define ZIPFILES_MQ_GET_FILE_LIST_H
#include "common.h"
#include "mp/mp.h"
namespace zipfiles::mp {

class GetFileListRequest : public Jsonable {
 public:
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  void setPath(std::string path);
  std::string getPath();

 private:
  std::string path;
};

class GetFileListResponse : public Jsonable {
 public:
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  void setFiles(std::vector<File> files);
  std::vector<File> getFiles();

 private:
  std::vector<File> files;
};

}  // namespace zipfiles::mp
#endif  // !ZIPFILES_MQ_GET_FILE_LIST_H
