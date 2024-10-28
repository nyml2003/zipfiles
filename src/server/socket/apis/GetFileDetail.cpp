#include <grp.h>
#include <mp/Request.h>
#include <mp/Response.h>
#include <pwd.h>
#include <server/socket/api.h>
#include <log4cpp/Category.hh>
namespace zipfiles::server::api {
namespace fs = std::filesystem;
template <>
response::GetFileDetail handle<request::GetFileDetail, response::GetFileDetail>(
  const request::GetFileDetail& request
) {
  const fs::path& path = request.path == "" ? "/" : request.path;
  const std::string& name = request.name;
  const fs::path file = path / name;

  if (!fs::exists(file)) {
    throw std::runtime_error("File does not exist");
  }

  struct stat file_stat {};
  if (lstat(file.c_str(), &file_stat) != 0) {
    throw std::runtime_error("Failed to get file details");
  }

  struct passwd* pwd = getpwuid(file_stat.st_uid);
  struct group* grp = getgrgid(file_stat.st_gid);

  fs::file_status file_status = fs::symlink_status(file);
  response::GetFileDetail res;
  res.type = file_status.type();
  res.createTime = static_cast<double>(file_stat.st_ctime);
  res.updateTime = static_cast<double>(file_stat.st_mtime);
  res.size = file_stat.st_size;
  res.owner = pwd ? pwd->pw_name : "";
  res.group = grp ? grp->gr_name : "";
  res.mode = file_stat.st_mode;
  res.name = name;
  res.path = path;
  return res;
}

}  // namespace zipfiles::server::api