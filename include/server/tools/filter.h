#ifndef ZIPFILES_METADATAFILTER_H
#define ZIPFILES_METADATAFILTER_H
#include <sys/types.h>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include "server/tools/fsapi.h"

namespace fs = std::filesystem;

namespace zipfiles::server {

class MetaDataFilter {
 public:
  using FilterFunc = std::function<bool(const FileDetail&)>;

  MetaDataFilter& filterByType(const fs::file_type& type);
  MetaDataFilter& filterBySize(const __off_t& minSize, const __off_t& maxSize);
  MetaDataFilter& filterByCreateTime(const double& min, const double& max);
  MetaDataFilter& filterByUpdateTime(const double& min, const double& max);
  MetaDataFilter& filterByOwner(const std::string& owner);
  MetaDataFilter& filterByGroup(const std::string& group);

  [[nodiscard]] bool doFilter(const FileDetail& fd) const;

 private:
  std::vector<FilterFunc> filters;
};

}  // namespace zipfiles::server

#endif  // ZIPFILES_METADATAFILTER_H