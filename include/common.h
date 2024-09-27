#ifndef ZIPFILES_COMMON_H
#define ZIPFILES_COMMON_H
#include <ctime>
#include <functional>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>
#include <vector>
#include <sys/select.h>

namespace zipfiles {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace fs = std::filesystem;

/**********************************

结构体

***********************************/

/**
 * @brief 用于展示的文件结构体
 *
 */
struct File {
  // 文件名
  std::string name;
  // 文件类型，定义见filesystem
  fs::file_type type;
};

/**
 * @brief 选中文件后展示的文件信息
 *
 */
struct FileDetail {
  // 文件类型，定义见filesystem
  fs::file_type type;
  // 创建时间
  std::string createTime;
  // 修改时间
  std::string updateTime;
  // 大小
  __off_t size;
  // 创建用户
  std::string owner;
  // 用户属于的用户组
  std::string group;
  // 权限
  mode_t mode;
  // 路径
  std::string path;
  // 文件名
  std::string name;
};

/**
 * @brief 用以展示目录结构的结构体
 *
 */
struct DirectoryTreeNode {
  // 文件名
  std::string name;
  // 文件类型，定义见filesystem
  fs::file_type type;
  // 子节点
  std::vector<struct DirectoryTreeNode> children;
};

/**
 * @brief 记录commit的日志结构
 *
 */
struct CommitLog {
  // Commit消息，默认为Commit的唯一标识
  std::string message;
  // 创建时间
  std::string createTime;
  // 恢复的默认路径
  std::string defaultPath;
  // 作者
  std::string author;
};

/**********************************

类

***********************************/

class MetaDataFilter {
 public:
  using FilterFunc = std::function<bool(const FileDetail&)>;

  MetaDataFilter& filterByType(fs::file_type type) {
    filters.emplace_back([&type](FileDetail& fd) { return fd.type == type; });
    return *this;
  }

  MetaDataFilter& filterBySize(__off_t minSize, __off_t maxSize) {
    filters.emplace_back([&minSize, &maxSize](FileDetail& fd) {
      return fd.size >= minSize && fd.size <= maxSize;
    });
    return *this;
  }

  MetaDataFilter& filterByOwner(const std::string& owner) {
    filters.emplace_back([&owner](FileDetail& fd) { return fd.owner == owner; }
    );
    return *this;
  }

  MetaDataFilter& filterByGroup(const std::string& group) {
    filters.emplace_back([&group](FileDetail& fd) { return fd.group == group; }
    );
    return *this;
  }

  [[nodiscard]] bool doFilter(const FileDetail& fd) const {
    for (const auto& filter : filters) {
      if (filter(fd)) {
        continue;
      }
      return false;
    }
    return true;
  }

 private:
  std::vector<FilterFunc> filters;
};

}  // namespace zipfiles

#endif  // ! ZIPFILES_COMMON_H
