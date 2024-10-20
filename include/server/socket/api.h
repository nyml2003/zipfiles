#ifndef ZIPFILE_SERVER_SOCKET_API_H
#define ZIPFILE_SERVER_SOCKET_API_H

namespace zipfiles::server::api {
template <typename RequestType, typename ResponseType>
ResponseType handle(const RequestType& request);
}

#endif  // ZIPFILE_SERVER_SOCKET_API_H