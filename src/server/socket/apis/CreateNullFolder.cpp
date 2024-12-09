#include <filesystem>
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/apis/CreateNullFolder.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"

namespace zipfiles::server::api {

void createNullFolder(int client_fd, const Req& req) {
  const auto& createNullFolder = std::get<request::CreateNullFolder>(req.kind);
  auto path = createNullFolder.path;
  auto newFolderName = createNullFolder.newFolderName;
  fs::path folderPath = path + "/" + newFolderName;
  if (folderPath.string().empty() ||
      folderPath.string().find("..") != std::string::npos) {
    Res res{
      response::NoResponse{"创建文件夹失败", "文件夹名不合法"}, req.uuid,
      Code::SERVER_ERROR
    };
    Socket::send(client_fd, res);
    return;
  }
  if (!fs::exists(path)) {
    Res res{
      response::NoResponse{"创建文件夹失败", "路径不存在"}, req.uuid,
      Code::SERVER_ERROR
    };
    Socket::send(client_fd, res);
    return;
  }

  if (!fs::is_directory(path)) {
    Res res{
      response::NoResponse{"创建文件夹失败", "路径不是文件夹"}, req.uuid,
      Code::SERVER_ERROR
    };
    Socket::send(client_fd, res);
    return;
  }

  auto pathStatus = fs::status(path);
  auto permissions = pathStatus.permissions();
  if ((permissions & fs::perms::owner_write) == fs::perms::none) {
    Res res{
      response::NoResponse{"创建文件夹失败", "没有权限"}, req.uuid,
      Code::SERVER_ERROR
    };
    Socket::send(client_fd, res);
    return;
  }

  if (fs::exists(folderPath)) {
    Res res{
      response::NoResponse{"创建文件夹失败", "文件夹已存在"}, req.uuid,
      Code::SERVER_ERROR
    };
    Socket::send(client_fd, res);
    return;
  }

  try {
    bool isSuccess = fs::create_directory(folderPath);
    if (!isSuccess) {
      Res res{
        response::NoResponse{"创建文件夹失败", "未知错误"}, req.uuid,
        Code::SERVER_ERROR
      };
      Socket::send(client_fd, res);
      return;
    }
  } catch (const fs::filesystem_error& e) {
    Res res{
      response::NoResponse{"创建文件夹失败", e.what()}, req.uuid,
      Code::SERVER_ERROR
    };
    Socket::send(client_fd, res);
    return;
  }

  Res res{response::CreateNullFolder{}, req.uuid, Code::OK};
  Socket::send(client_fd, res);
}

}  // namespace zipfiles::server::api