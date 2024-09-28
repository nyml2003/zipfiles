#include <log4cpp/Category.hh>
#include "common.h"
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/filter.h"
#include "server/handler.h"
#include "server/socket/socket.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {

void doHandle() {
  try {
    // 主eventloop
    while (true) {
      log4cpp::Category::getRoot().infoStream() << "Waiting for request";
      ReqPtr request = Socket::receive();

      log4cpp::Category::getRoot().infoStream()
        << "Request received: " << request->toJson().toStyledString();

      ResPtr response = std::visit(
        overload{
          [](request::GetFileDetail& req) {
            return makeResGetFileDetail(getFileDetail(req.path));
          },
          [](request::GetFileList& req) {
            MetaDataFilter filter;
            return makeResGetFileList(getFileList(req.path, false, filter));
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

      Socket::send(response);

      log4cpp::Category::getRoot().infoStream()
        << "Response sent: " << response->toJson().toStyledString();
    }
  } catch (const std::exception& e) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to handle request: " << e.what();
  }
}  // namespace zipfiles::server
}  // namespace zipfiles::server