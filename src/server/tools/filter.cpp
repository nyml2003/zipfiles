#include "server/tools/filter.h"

namespace zipfiles::server {

MetaDataFilter& MetaDataFilter::filterByType(const fs::file_type& type) {
  filters.emplace_back([&type](const FileDetail& fd) { return fd.type == type; }
  );
  return *this;
}

MetaDataFilter&
MetaDataFilter::filterBySize(const __off_t& minSize, const __off_t& maxSize) {
  filters.emplace_back([&minSize, &maxSize](const FileDetail& fd) {
    return fd.size >= minSize && fd.size <= maxSize;
  });
  return *this;
}

MetaDataFilter&
MetaDataFilter::filterByCreateTime(const double& min, const double& max) {
  filters.emplace_back([&min, &max](const FileDetail& fd) {
    return fd.createTime >= min && fd.createTime <= max;
  });
  return *this;
}

MetaDataFilter&
MetaDataFilter::filterByUpdateTime(const double& min, const double& max) {
  filters.emplace_back([&min, &max](const FileDetail& fd) {
    return fd.updateTime >= min && fd.updateTime <= max;
  });
  return *this;
}

MetaDataFilter& MetaDataFilter::filterByOwner(const std::string& owner) {
  filters.emplace_back([&owner](const FileDetail& fd) {
    return fd.owner == owner;
  });
  return *this;
}

MetaDataFilter& MetaDataFilter::filterByGroup(const std::string& group) {
  filters.emplace_back([&group](const FileDetail& fd) {
    return fd.group == group;
  });
  return *this;
}

bool MetaDataFilter::doFilter(const FileDetail& fd) const {
  return std::all_of(filters.begin(), filters.end(), [&fd](const auto& filter) {
    return filter(fd);
  });
}

}  // namespace zipfiles::server