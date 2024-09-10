#include "mp/GetFileList.h"

namespace zipfiles::mp {
Json::Value GetFileListResponse::toJson() {
  Json::Value json;
  for (const auto& filename : filenames) {
    json["filenames"].append(filename);
  }
  return json;
}

void GetFileListResponse::fromJson(const Json::Value& json) {
  filenames.clear();
  for (const auto& filename : json["filenames"]) {
    filenames.push_back(filename.asString());
  }
}

void GetFileListResponse::setFilenames(std::vector<std::string> filenames) {
  this->filenames = filenames;
}

std::vector<std::string> GetFileListResponse::getFilenames() {
  return filenames;
}

}  // namespace zipfiles::mq::GetFileList