#include "client/api.h"
#include "client/socket.h"
#include <iostream>
#include "mp/Response.h"

namespace zipfiles::client {

ResPtr doHandle(const ReqPtr& request) {
  try {
    socket::Socket::send(request);

    ResPtr response = socket::Socket::receive();

    if (response->status != StatusCode::OK) {
      std::cerr << "Response status is not OK." << std::endl;
    }

    return response;

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return nullptr;
}

}  // namespace zipfiles::client