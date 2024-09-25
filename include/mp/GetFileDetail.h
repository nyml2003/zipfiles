#ifndef ZIPFILES_MP_GET_FILE_DETAIL_H
#define ZIPFILES_MP_GET_FILE_DETAIL_H
#include "common.h"
#include "mp/mp.h"
namespace zipfiles::mp {

class GetFileDetailRequest : public Jsonable {
 public:
  explicit GetFileDetailRequest() = default;
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  void setPath(const std::string& path);
  std::string getPath();

 private:
  std::string path;
};

using GetFileDetailRequestPtr = std::shared_ptr<GetFileDetailRequest>;

class GetFileDetailResponse : public Jsonable {
 public:
  explicit GetFileDetailResponse() : metadata() {}
  Json::Value toJson() override;
  void fromJson(const Json::Value& json) override;
  void setMetadata(const FileDetail& metadata);
  FileDetail getMetadata();

 private:
  FileDetail metadata;
};

using GetFileDetailResponsePtr = std::shared_ptr<GetFileDetailResponse>;

}  // namespace zipfiles::mp

#endif  // !ZIPFILES_MP_GET_FILE_DETAIL_H