#ifndef ZIPFILES_MP_APIS_GET_COMMIT_RECYCLE_BIN_H
#define ZIPFILES_MP_APIS_GET_COMMIT_RECYCLE_BIN_H

#include <string>
#include <vector>

namespace zipfiles {

namespace request {
struct GetCommitRecycleBin {};
}  // namespace request

namespace response {
struct getCommitRecycleBin {
  struct CommitLog {
    // id为Commit的唯一标识
    std::string uuid;
    // Commit消息
    std::string message;
    // 创建时间
    // 以秒为单位，有效位数是53位
    double createTime;
    // 文件的存储路径
    std::string storagePath;
    // 作者
    std::string author;
    // 是否被加密
    bool isEncrypt;
    // 是否被删除
    bool isDelete;
  };
};

struct GetCommitRecycleBin {
  std::vector<getCommitRecycleBin::CommitLog> commits;
};
}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_GET_COMMIT_RECYCLE_BIN_H