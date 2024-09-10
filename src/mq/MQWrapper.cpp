#include "mq/MQWrapper.h"
namespace zipfiles::mq {

const std::string MQWrapper::MQ_NAME = "/my_mq";

MQWrapper::MQWrapper() {
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;

  mq = mq_open(MQ_NAME.c_str(), O_CREAT | O_RDWR, 0644, &attr);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
  }
}

MQWrapper::~MQWrapper() {
  mq_close(mq);
  mq_unlink(MQ_NAME.c_str());
}

bool MQWrapper::sendRequest(const Request& req) {
  if (mq_send(mq, (const char*)&req, sizeof(req), 0) == -1) {
    perror("mq_send");
    return false;
  }
  return true;
}

bool MQWrapper::receiveRequest(Request& req) {
  ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
  if (bytes_read == -1) {
    perror("mq_receive");
    return false;
  }
  std::memcpy(&req, buffer, sizeof(req));
  return true;
}

bool MQWrapper::sendResponse(const Response& res) {
  if (mq_send(mq, (const char*)&res, sizeof(res), 0) == -1) {
    perror("mq_send");
    return false;
  }
  return true;
}

bool MQWrapper::receiveResponse(Response& res) {
  ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
  if (bytes_read == -1) {
    perror("mq_receive");
    return false;
  }
  std::memcpy(&res, buffer, sizeof(res));
  return true;
}
}  // namespace zipfiles::mq