#ifndef ZIPFILES_CLIENT_REQUEST_H
#define ZIPFILES_CLIENT_REQUEST_H
#include "mq/mq.h"
#include "mq/MQWrapper.h"
namespace zipfiles::client::request {
    template <typename T>
    class Request {
    public:
     explicit Request(
       zipfiles::mq::ApiType api,
       zipfiles::mq::MQWrapper& mqWrapper
     );
     T handle();

    private:
        zipfiles::mq::ApiType api;
        zipfiles::mq::MQWrapper& mqWrapper;
    };
}
#endif  // !ZIPFILES_CLIENT_REQUEST_H