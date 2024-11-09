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
    throw std::runtime_error("File does not exist");
  }

  if (!fs::is_directory(path)) {
    throw std::runtime_error("Path is not a directory");
  }

  response::GetFileList response;
  for (const auto& entry : fs::directory_iterator(path)) {
    response.files.push_back(
      {.type = entry.status().type(), .name = entry.path().filename().string()}
    );
  }
  Socket::send(
    client_fd, Res(response, Api::GET_FILE_LIST, req.uuid, Code::OK)
  );
}

}  // namespace zipfiles::server::api