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
        api::getCommitDetail(client_fd, req);
        break;
      }
      case Api::GET_COMMIT_LIST: {
        api::getCommitList(client_fd, req);
        break;
      }
      case Api::GET_FILE_LIST: {
        api::getFileList(client_fd, req);
        break;
      }
      case Api::GET_FILE_DETAIL_LIST: {
        api::getFileDetailList(client_fd, req);
        break;
      }
      case Api::POST_COMMIT: {
        api::postCommit(client_fd, req);
        break;
      }
      case Api::GET_FILE_DETAIL: {
        api::getFileDetail(client_fd, req);
        break;
      }
      case Api::MOCK_NEED_TIME: {
        api::mockNeedTime(client_fd, req);
        break;
      }
      case Api::MOCK_MANY_NOTIFICATIONS: {
        api::mockManyNotifications(client_fd, req);
        break;
      }
      case Api::RESTORE:
        api::restore(client_fd, req);
        break;
      case Api::LOGIC_DELETE_COMMIT:
        api::logicDeleteCommit(client_fd, req);
        break;
      case Api::PHYSICAL_DELETE_COMMIT:
        api::physicalDeleteCommit(client_fd, req);
        break;
      case Api::GET_COMMIT_RECYCLE_BIN:
        api::getCommitRecycleBin(client_fd, req);
        break;
      case Api::RECOVER_COMMIT:
        api::recoverCommit(client_fd, req);
        break;
      default:
        sendError(client_fd, "Unknown api type");
    }
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
    Socket::send(
      client_fd,
      Res(
        response::NoResponse(), Api::NORESPONSE, req.uuid, Code::ERROR, e.what()
      )
    );
  }
}  // namespace zipfiles::server
}  // namespace zipfiles::server