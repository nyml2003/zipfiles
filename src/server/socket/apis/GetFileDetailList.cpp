
#include "mp/Request.h"
#include "mp/Response.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"

#include <grp.h>
#include <pwd.h>
#include <algorithm>
#include <filesystem>
#include <log4cpp/Category.hh>
#include <vector>

namespace zipfiles::server::api {

void getFileDetailList(int client_fd, const Req& req) {
  const auto& request = std::get<request::GetFileDetailList>(req.kind);
  const fs::path& path = request.path == "" ? "/" : request.path;
  if (!fs::exists(path)) {
    Socket::send(
      client_fd, Res(
                   response::NoResponse{
                     .title = "路径不存在",
                     .description = "路径" + path.string() + "不存在"
                   },
                   req.uuid, Code::SERVER_ERROR
                 )
    );
    return;
  }
  if (!fs::is_directory(path)) {
    Socket::send(
      client_fd, Res(
                   response::NoResponse{
                     .title = "路径不是目录",
                     .description = "路径" + path.string() + "不是目录"
                   },
                   req.uuid, Code::SERVER_ERROR
                 )
    );
    return;
  }
  const auto& filter = request.filter;
  std::vector<response::getFileDetailList::FileDetail> files;
  for (const auto& entry : fs::directory_iterator(path)) {
    const auto& file = entry.path();
    struct stat file_stat {};
    if (lstat(file.c_str(), &file_stat) != 0) {
      Socket::send(
        client_fd, Res(
                     response::NoResponse{
                       .title = " 文件不存在",
                       .description = "文件" + file.string() + "的元数据不存在"
                     },
                     req.uuid, Code::SERVER_ERROR
                   )
      );
      return;
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
       .path = file.parent_path(),
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
  std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
    // 优先按照文件类型排序，目录在前
    if (a.type != b.type) {
      if (a.type == fs::file_type::directory) {
        return true;
      }
      if (b.type == fs::file_type::directory) {
        return false;
      }
    }
    // 其次按照字典序排序
    return a.name < b.name;
  });
  Socket::send(
    client_fd,
    Res(response::GetFileDetailList{.files = files}, req.uuid, Code::OK)
  );
}

}  // namespace zipfiles::server::api