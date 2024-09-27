#ifndef ZIPFILE_SERVER_API_H
#define ZIPFILE_SERVER_API_H
#include "mp/Request.h"
#include "mp/Response.h"
// 调用server能力, 解包请求, 打包响应
namespace zipfiles::server::api {
ResPtr handleRequest(const ReqPtr& req);
}  // namespace zipfiles::server::api

#endif  // !ZIPFILE_CLIENT_API_H