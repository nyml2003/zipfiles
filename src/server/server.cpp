#include "mp/mp.h"
#include "common.h"
#include "mp/GetFileList.h"
#include "server/tools/fsapi.h"
#include <iostream>

using namespace zipfiles::mp;

Response getFileList(Request& request, ServerSocket& socket) {
  GetFileListRequest getFileListRequest;
  getFileListRequest.fromJson(request.getPayload());

  std::vector<zipfiles::File> files =
    zipfiles::server::list(getFileListRequest.getPath());
  Response response;
  response.setStatus(StatusCode::OK);
  GetFileListResponse getFileListResponse;
  getFileListResponse.setFiles(files);
  response.setPayload(getFileListResponse);
  return response;
}

int main() {
  ServerSocket server;

  while (true) {
    server.acceptConnection();
    Request req;
    try {
      while (server.receive(req)) {
        if (req.is(ApiType::GET_FILE_LIST)) {
          Response res = getFileList(req, server);
          server.send(res);
        }
      }
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
    std::cout << "Waiting for new client connection..." << std::endl;
  }

  return 0;
}
