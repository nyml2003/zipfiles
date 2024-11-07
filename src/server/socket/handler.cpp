#include "server/socket/handler.h"

#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/common.h"
#include "server/error.h"
#include "server/socket/api.h"
#include "server/socket/socket.h"

#include <unistd.h>
#include <csignal>
#include <log4cpp/Category.hh>

namespace zipfiles::server {
/**
 * @brief handler实现
 *
 * @param client_fd 发送来请求的client，用于回传response
 *
 * @param request selector解析出的request
 *
 */
void doHandle(int client_fd, const Req& req) {
  try {
    log4cpp::Category::getRoot().infoStream()
      << "Request received: " << req.uuid << " " << static_cast<int>(req.api)
      << req.kind.index();
    // 主eventloop
    ResKind kind;

    switch (req.api) {
      case Api::GET_COMMIT_DETAIL: {
        kind = api::handle<request::GetCommitDetail, response::GetCommitDetail>(
          std::get<request::GetCommitDetail>(req.kind)
        );
        break;
      }
      case Api::GET_COMMIT_LIST: {
        kind = api::handle<request::GetCommitList, response::GetCommitList>(
          std::get<request::GetCommitList>(req.kind)
        );
        break;
      }
      case Api::GET_FILE_LIST: {
        kind = api::handle<request::GetFileList, response::GetFileList>(
          std::get<request::GetFileList>(req.kind)
        );
        break;
      }
      case Api::GET_FILE_DETAIL_LIST: {
        kind =
          api::handle<request::GetFileDetailList, response::GetFileDetailList>(
            std::get<request::GetFileDetailList>(req.kind)
          );
        break;
      }
      case Api::POST_COMMIT: {
        kind = api::handle<request::PostCommit, response::PostCommit>(
          std::get<request::PostCommit>(req.kind)
        );
        break;
      }
      case Api::GET_FILE_DETAIL: {
        kind = api::handle<request::GetFileDetail, response::GetFileDetail>(
          std::get<request::GetFileDetail>(req.kind)
        );
        break;
      }
      case Api::MOCK_NEED_TIME: {
        kind = api::handle<request::MockNeedTime, response::MockNeedTime>(
          std::get<request::MockNeedTime>(req.kind)
        );
        break;
      }
      default:
        throw std::runtime_error("Unknown api type");
    }
    Res res = {kind, req.api, req.uuid, Code::OK, std::nullopt};

    log4cpp::Category::getRoot().infoStream()
      << "Response sent: " << res.toJson();
    Socket::send(client_fd, res);

  }  // namespace zipfiles::server
  catch (const std::exception& e) {
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
}  // namespace zipfiles::server