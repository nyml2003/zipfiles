#ifndef ZIPFILE_CLIENT_API_H
#define ZIPFILE_CLIENT_API_H
#include "mp/Request.h"
#include "mp/Response.h"
namespace zipfiles::client {
ResPtr doHandle(const ReqPtr& request);

}  // namespace zipfiles::client

#endif  // !ZIPFILE_CLIENT_API_H