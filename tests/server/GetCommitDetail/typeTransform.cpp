
#include "mp/Request.h"
#include "mp/Response.h"
#include "mp/apis/GetCommitDetail.h"
#include "server/socket/api.h"

#include <iostream>

int main() {
  zipfiles::request::GetCommitDetail req;
  req.uuid = "bdc285a3-d569-47f0-b379-d9e9dce80642";
  zipfiles::response::GetCommitDetail res = zipfiles::server::api::handle<
    zipfiles::request::GetCommitDetail, zipfiles::response::GetCommitDetail>(req
  );
  std::cout << "Response: " << zipfiles::Res(res).toJson() << std::endl;
}