#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <server/socket/socket.h>
#include <log4cpp/Category.hh>

#include <algorithm>

namespace zipfiles::server::api {
namespace fs = std::filesystem;
void getFileList(int client_fd, const Req& req) {
  const auto& request = std::get<request::GetFileList>(req.kind);
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

  response::GetFileList response;
  for (const auto& entry : fs::directory_iterator(path)) {
    response.files.push_back(
      {.type = entry.status().type(), .name = entry.path().filename().string()}
    );
  }
  std::sort(
    response.files.begin(), response.files.end(),
    [](const auto& a, const auto& b) {
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
    }
  );
  Socket::send(client_fd, Res(response, req.uuid, Code::OK));
}

}  // namespace zipfiles::server::api