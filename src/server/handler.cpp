#include "server/handler.h"
#include <iostream>
#include "common.h"
#include "mp/Request.h"
#include "mp/Response.h"
#include "server/socket/socket.h"
#include "server/tools/fsapi.h"

namespace zipfiles::server {

void doHandle() {
  try {
    // 主eventloop
    while (true) {
      ReqPtr request = Socket::receive();
      ResPtr response = std::visit(
        overload{
          [](const request::GetFileDetail& req) {
            return makeResGetFileDetail(getFileDetail(req.path));
          },
          [](const request::GetFileList& req) {
            return makeResGetFileList(getFileList(req.path));
          },
          [](auto&&) {
            throw std::runtime_error("Unknown request type");
            return nullptr;
          },
        },
        request->kind
      );

      response->status = StatusCode::OK;
      Socket::send(response);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}  // namespace zipfiles::server
}  // namespace zipfiles::server