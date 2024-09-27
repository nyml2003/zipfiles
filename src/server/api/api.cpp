#include "server/api/api.h"
#include "mp/Request.h"
#include "mp/Response.h"
#include "common.h"
#include "server/tools/fsapi.h"
namespace zipfiles::server::api {
ResPtr handleRequest(const ReqPtr& req) {
  return std::visit(
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
      }},
    req->kind
  );
}
}  // namespace zipfiles::server::api