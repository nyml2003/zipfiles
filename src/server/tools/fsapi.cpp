#include "server/tools/fsapi.h"
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <log4cpp/Category.hh>
#include <vector>
#include "server/tools/filter.h"

namespace zipfiles::server {
/**
 * @brief 获取选定文件的元信息
 *
 * @param file 指定的文件路径
 *
 * @return 返回一个FileDetail类型
 *
 */
FileDetail getFileDetail(const fs::path& file) {
  log4cpp::Category::getRoot().infoStream()
    << "Getting file detail for " << file;

  if (!fs::exists(file)) {
    throw std::runtime_error("File does not exist");
  }

  struct stat file_stat {};
  if (lstat(file.c_str(), &file_stat) != 0) {
    throw std::runtime_error("Failed to get file details");
  }

  struct passwd* pwd = getpwuid(file_stat.st_uid);
  struct group* grp = getgrgid(file_stat.st_gid);

  fs::file_status file_status = fs::symlink_status(file);

  return {
    .type = file_status.type(),
    .createTime = static_cast<double>(file_stat.st_ctime),
    .updateTime = static_cast<double>(file_stat.st_mtime),
    .size = file_stat.st_size,
    .owner = pwd ? pwd->pw_name : "",
    .group = grp ? grp->gr_name : "",
    .mode = file_stat.st_mode,
    .absolutePath = file,
    .dev = file_stat.st_dev};
}

/**
 * @brief 获取选定文件的元信息
 *
 * @param directory 指定的目录路径
 *
 * @return 返回一个FileDetail类型的数组
 *
 */
std::vector<FileDetail> getAllFileDetails(
  const fs::path& directory,
  bool doFilter,
  MetaDataFilter& filter
) {
  if (!fs::exists(directory) || !fs::is_directory(directory)) {
    throw std::runtime_error("Directory does not exist or is not a directory");
  }

  std::vector<FileDetail> file_details;

  for (const auto& entry : fs::directory_iterator(directory)) {
    FileDetail fd = getFileDetail(entry.path());

    if (doFilter) {
      if (filter.doFilter(fd)) {
        file_details.push_back(fd);
      }
    } else {
      file_details.push_back(fd);
    }
  }

  return file_details;
}

}  // namespace zipfiles::server