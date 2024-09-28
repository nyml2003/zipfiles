#include "client/api.h"
#include "client/socket.h"
#include "log4cpp/Category.hh"
#include "mp/Request.h"
#include "mp/Response.h"

namespace zipfiles::client {

ResPtr doHandle(const ReqPtr& request) {
  log4cpp::Category::getRoot().infoStream()
    << "Sending request to server: " << request->toJson().toStyledString();
  socket::Socket::getInstance().send(request);
  ResPtr response = socket::Socket::getInstance().receive();
  if (response->status != StatusCode::OK) {
    log4cpp::Category::getRoot().errorStream()
      << "Failed to handle request: " << response->status;
    throw std::runtime_error("Failed to handle request");
  }
  return response;
}

}  // namespace zipfiles::client