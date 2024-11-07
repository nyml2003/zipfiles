#include <mp/Request.h>
#include <mp/Response.h>
#include <server/socket/api.h>

namespace zipfiles::server::api {

template <>
response::MockNeedTime handle<request::MockNeedTime, response::MockNeedTime>(
  const request::MockNeedTime& req
) {
  response::MockNeedTime response;
  response.id = req.id;
  return response;
}

}  // namespace zipfiles::server::api