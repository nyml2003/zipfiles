#include "client/request.h"
#include <stdexcept>
#include "mq/MQWrapper.h"

namespace zipfiles::client::request {
    template <typename T>
    Request<T>::Request(zipfiles::mq::ApiType api) : api(api) {}

    template <typename T>
    T Request<T>::handle() {
        if (!mqWrapper.sendRequest(req)) {
            throw std::runtime_error("Failed to send request");
        }

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
}  // namespace zipfiles::client::request