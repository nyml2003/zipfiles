#ifndef ZIPFILES_MP_GET_FILE_LIST_H
#define ZIPFILES_MP_GET_FILE_LIST_H
#include "common.h"
#include "mp/mp.h"
namespace zipfiles::mp {

class GetFileListRequest : public Jsonable {
 public:
  explicit GetFileListRequest() = default;
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  void setPath(std::string path);
  std::string getPath();

 private:
  std::string path;
};

using GetFileListRequestPtr = std::shared_ptr<GetFileListRequest>;

class GetFileListResponse : public Jsonable {
 public:
  explicit GetFileListResponse() = default;
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  void setFiles(const std::vector<File>& files);
  std::vector<File> getFiles();

 private:
  std::vector<File> files;
};

using GetFileListResponsePtr = std::shared_ptr<GetFileListResponse>;

}  // namespace zipfiles::mp
#endif  // !ZIPFILES_MP_GET_FILE_LIST_H
