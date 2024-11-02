#include "server/handler.h"

#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/apis/GetCommitDetail.h"
#include "mp/apis/GetFileDetail.h"
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
void doHandle(int client_fd, const ReqPtr& request) {
  try {
    // 主eventloop

    log4cpp::Category::getRoot().infoStream()
      << "Request received: " << request->toJson();

    ResPtr response = std::visit(
      overload{
        [](request::GetFileList& req) {
          return std::make_shared<Res>(
            api::handle<request::GetFileList, response::GetFileList>(req)
          );
        },
        [](request::GetFileDetailList& req) {
          const response::GetFileDetailList res = api::handle<
            request::GetFileDetailList, response::GetFileDetailList>(req);
          log4cpp::Category::getRoot().infoStream()
            << "Response: " << res.files.size();
          return std::make_shared<Res>(res);
        },
        [](request::PostCommit& req) {
          return std::make_shared<Res>(
            api::handle<request::PostCommit, response::PostCommit>(req)
          );
        },
        [](request::GetCommitDetail& req) {
          return std::make_shared<Res>(
            api::handle<request::GetCommitDetail, response::GetCommitDetail>(req
            )
          );
        },
        [](request::GetCommitList& req) {
          log4cpp::Category::getRoot().infoStream() << "GetCommitList";
          return std::make_shared<Res>(
            api::handle<request::GetCommitList, response::GetCommitList>(req)
          );
        },
        [](request::MockNeedTime& req) {
          return std::make_shared<Res>(response::MockNeedTime{req.id});
        },
        [](request::GetFileDetail& req) {
          return std::make_shared<Res>(
            api::handle<request::GetFileDetail, response::GetFileDetail>(req)
          );
        }
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