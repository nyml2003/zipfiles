#ifndef ZIPFILES_SERVER_ERROR_H
#define ZIPFILES_SERVER_ERROR_H

#include <stdexcept>
#include <string>
namespace zipfiles::server {

class SocketTemporarilyUnavailable : public std::runtime_error {
 public:
  explicit SocketTemporarilyUnavailable(const std::string& what)
    : std::runtime_error("Socket temporarily unavailable: " + what) {}
};

}  // namespace zipfiles::server
#endif  // !ZIPFILES_MP_ERROR_H