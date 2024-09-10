#ifndef ZIPFILES_CLIENT_REQUEST_H
#define ZIPFILES_CLIENT_REQUEST_H
#include "mq/mq.h"
namespace zipfiles::client::request {
    template <typename T>
    class Request {
    public:
     explicit Request(zipfiles::mq::ApiType api);
     T handle();

    private:
        zipfiles::mq::ApiType api;
    };
}
#endif  // !ZIPFILES_CLIENT_REQUEST_H