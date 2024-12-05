#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>
#include <server/socket/socket.h>
#include <log4cpp/Category.hh>

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
  Socket::send(client_fd, Res(response, req.uuid, Code::OK));
}

}  // namespace zipfiles::server::api