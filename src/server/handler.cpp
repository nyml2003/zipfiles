#include <unistd.h>
#include <csignal>
#include <log4cpp/Category.hh>
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/common.h"
#include "mp/error.h"
#include "mp/filter.h"
#include "server/handler.h"
#include "server/socket/socket.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {

void doHandle(int client_fd) {
  try {
    // 主eventloop
    // log4cpp::Category::getRoot().infoStream()
    //   << "Thread " << std::this_thread::get_id()
    //   << " is waiting for request from " << client_fd << "...";

    ReqPtr request = Socket::receive(client_fd);

    log4cpp::Category::getRoot().infoStream()
      << "Request received: " << request->toJson();

    ResPtr response = std::visit(
      overload{
        [](request::GetFileDetail& req) {
          return makeResGetFileDetail(getFileDetail(req.path));
        },
        [](request::GetFileList& req) {
          MetaDataFilter filter;
          bool needFilter = false;
          if (req.filter.has_value()) {
            needFilter = true;
            if (req.filter->type.has_value()) {
              filter.filterByType(req.filter->type.value());
            }
            if (req.filter->minSize.has_value() &&
                req.filter->maxSize.has_value()) {
              filter.filterBySize(
                req.filter->minSize.value(), req.filter->maxSize.value()
              );
            }
            if (req.filter->minSize.has_value() &&
                !req.filter->maxSize.has_value()) {
              filter.filterBySize(req.filter->minSize.value(), INT64_MAX);
            }
            if (!req.filter->minSize.has_value() &&
                req.filter->maxSize.has_value()) {
              filter.filterBySize(0, req.filter->maxSize.value());
            }
            if (req.filter->owner.has_value()) {
              filter.filterByOwner(req.filter->owner.value());
            }
            if (req.filter->group.has_value()) {
              filter.filterByGroup(req.filter->group.value());
            }
          }
          return makeResGetFileList(getFileList(req.path, needFilter, filter));
        },
        [](request::PostCommit&) { return makeResPostCommit({}); },
        [](request::MockNeedTime& req) { return makeResMockNeedTime(req.id); },
        [](request::GetAllFileDetails& req) {
          return makeResGetAllFileDetails(getAllFileDetails(req.path));
        },
        [](auto&&) {
          throw std::runtime_error("Unknown request type");
          return nullptr;
        },
      },
      request->kind
    );

    // 设置response
    response->status = StatusCode::OK;
    response->timestamp = request->timestamp;
    response->uuid = request->uuid;
    log4cpp::Category::getRoot().infoStream()
      << "Response sent: " << response->toJson();
    Socket::send(client_fd, response);

  } catch (const std::exception& e) {
    // 如果是SocketTemporarilyUnavailable
    if (const auto* e_ptr =
          dynamic_cast<const SocketTemporarilyUnavailable*>(&e)) {
      // log4cpp::Category::getRoot().infoStream() << e_ptr->what();
      return;
    }
    log4cpp::Category::getRoot().errorStream()
      << "Failed to handle request: " << e.what();
  }
}  // namespace zipfiles::server
/**
 * @brief 守护进程初始化函数
 * ? 待使用
 */
void daemonize() {
  pid_t pid = fork();
  if (pid < 0) {
    log4cpp::Category::getRoot().errorStream() << "Failed to fork process";
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);  // 父进程退出
  }

  // 创建新的会话
  if (setsid() < 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to create new session";
    exit(EXIT_FAILURE);
  }

  // 捕捉和处理信号
  if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to set signal handler for SIGCHLD";
    exit(EXIT_FAILURE);
  }
  if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to set signal handler for SIGTSTP";
    exit(EXIT_FAILURE);
  }

  // 更改工作目录
  if (chdir("/") < 0) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to change working directory";
    exit(EXIT_FAILURE);
  }

  // 关闭文件描述符
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}
}  // namespace zipfiles::server