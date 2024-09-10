#include "client/request.h"
#include "mq/mq.h"
#include <stdexcept>
#include <mq/GetFileList.h>

namespace zipfiles::client::request {
template <typename T>
Request<T>::Request(
  zipfiles::mq::ApiType api,
  zipfiles::mq::MQWrapper& mqWrapper
)
  : api(api), mqWrapper(mqWrapper) {}

template <typename T>
T Request<T>::handle() {
  zipfiles::mq::Request req;
  req.api = api;
  if (!mqWrapper.sendRequest(req)) {
    throw std::runtime_error("Failed to send request");
  }
  zipfiles::mq::Response res;

  if (!mqWrapper.receiveResponse(res)) {
    throw std::runtime_error("Failed to receive response");
  }

  if (res.status != zipfiles::mq::StatusCode::OK) {
    throw std::runtime_error("Error in response");
  }

  T content;
  size_t offset = 0;
  content.deserialize(res.content, offset);
  return content;
}
template class Request<zipfiles::mq::GetFileList::GetFileList>;
}  // namespace zipfiles::client::request