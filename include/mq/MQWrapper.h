#ifndef ZIPFILES_MQ_WRAPPER_H
#define ZIPFILES_MQ_WRAPPER_H

#include "mq/mq.h"
#include <mqueue.h>
#include <cstring>
#include <iostream>
namespace zipfiles::mq {
class MQWrapper {
 public:
  MQWrapper();
  ~MQWrapper();

  bool sendRequest(const Request& req);
  bool receiveRequest(Request& req);
  bool sendResponse(const Response& res);
  bool receiveResponse(Response& res);

 private:
  mqd_t mq;
  static const std::string MQ_NAME;
  struct mq_attr attr;
  char buffer[MAX_MSG_SIZE];
};
}  // namespace zipfiles::mq
#endif  // MQ_WRAPPER_H