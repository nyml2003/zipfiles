#ifndef ZIPFILES_MP_APIS_POST_COMMIT_H
#define ZIPFILES_MP_APIS_POST_COMMIT_H

#include <optional>
#include <string>
#include <vector>

namespace zipfiles {

namespace request {
struct PostCommit {
  // 包含的文件数组
  std::vector<std::string> files;
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
  // 加密密钥
  std::optional<std::string> key;
};

}  // namespace request

namespace response {

struct PostCommit {};

}  // namespace response

}  // namespace zipfiles

#endif  // !ZIPFILES_MP_APIS_POST_COMMIT_H