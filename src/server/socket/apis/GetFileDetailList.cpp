#include <grp.h>
#include <mp/Request.h>
#include <mp/Response.h>
#include <pwd.h>
#include <server/socket/api.h>
#include <filesystem>
#include <log4cpp/Category.hh>
#include <vector>
namespace zipfiles::server::api {
template <>
response::GetFileDetailList
handle<request::GetFileDetailList, response::GetFileDetailList>(
  const request::GetFileDetailList& request
) {
  const fs::path& path = request.path == "" ? "/" : request.path;
  if (!fs::exists(path)) {
    throw std::runtime_error("File does not exist");
  }
  if (!fs::is_directory(path)) {
    throw std::runtime_error("Path is not a directory");
  }
  const auto& filter = request.filter;
  std::vector<response::getFileDetailList::FileDetail> files;
  for (const auto& entry : fs::directory_iterator(path)) {
    const auto& file = entry.path();
    struct stat file_stat {};
    if (lstat(file.c_str(), &file_stat) != 0) {
      throw std::runtime_error("Failed to get file details");
    }

    struct passwd* pwd = getpwuid(file_stat.st_uid);
    struct group* grp = getgrgid(file_stat.st_gid);
    files.push_back(
      {.type = entry.status().type(),
       .createTime = static_cast<double>(file_stat.st_ctime),
       .updateTime = static_cast<double>(file_stat.st_mtime),
       .size = file_stat.st_size,
       .owner = pwd ? pwd->pw_name : "",
       .group = grp ? grp->gr_name : "",
       .mode = file_stat.st_mode,
       .path = file.string(),
       .name = file.filename().string()}
    );
    if (filter.has_value()) {
      const auto& f = filter.value();
      if (f.type.has_value() && f.type.value() != entry.status().type()) {
        files.pop_back();
        continue;
      }
      if (f.minSize.has_value() && f.minSize.value() > file_stat.st_size) {
        files.pop_back();
        continue;
      }
      if (f.maxSize.has_value() && f.maxSize.value() < file_stat.st_size) {
        files.pop_back();
        continue;
      }
      if (f.minCreateTime.has_value() &&
          f.minCreateTime.value() > static_cast<double>(file_stat.st_ctime)) {
        files.pop_back();
        continue;
      }
      if (f.maxCreateTime.has_value() &&
          f.maxCreateTime.value() < static_cast<double>(file_stat.st_ctime)) {
        files.pop_back();
        continue;
      }
      if (f.minUpdateTime.has_value() &&
          f.minUpdateTime.value() > static_cast<double>(file_stat.st_mtime)) {
        files.pop_back();
        continue;
      }
      if (f.maxUpdateTime.has_value() &&
          f.maxUpdateTime.value() < static_cast<double>(file_stat.st_mtime)) {
        files.pop_back();
        continue;
      }
      if (f.owner.has_value() && f.owner.value() != files.back().owner) {
        files.pop_back();
        continue;
      }
      if (f.group.has_value() && f.group.value() != files.back().group) {
        files.pop_back();
        continue;
      }
    }
  }
  return {.files = files};
}

}  // namespace zipfiles::server::api