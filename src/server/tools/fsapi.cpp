#include "server/tools/fsapi.h"
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <log4cpp/Category.hh>
#include <vector>

namespace zipfiles::server {
/**
 * @brief 获取给定目录下的所有文件和目录(No follow，只返回一层的结果)
 *
 * @param directory 指定的目录路径
 *
 * @return 返回一个File类型的数组
 *
 */
std::vector<File>
getFileList(const fs::path& directory, bool doFilter, MetaDataFilter& filter) {
  const fs::path& directory_path = directory;

  log4cpp::Category::getRoot().infoStream()
    << "Getting files list for " << directory_path;

  if (!fs::exists(directory_path)) {
    throw std::runtime_error("Directory does not exist.");
  }

  if (!fs::is_directory(directory_path)) {
    throw std::runtime_error(directory_path.string() + " is not a directory.");
  }

  std::vector<File> files;

  for (const auto& entry : fs::directory_iterator(directory_path)) {
    File file;
    file.name = entry.path().filename().string();
    file.type = entry.symlink_status().type();

    if (doFilter) {
      FileDetail fd = getFileDetail(entry.path());
      if (filter.doFilter(fd)) {
        files.push_back(file);
      }
    } else {
      files.push_back(file);
    }
  }
  return files;
}

/**
 * @brief 获取选定文件的元信息
 *
 * @param file 指定的文件路径
 *
 * @return 返回一个FileDetail类型
 *
 */
FileDetail getFileDetail(const fs::path& file) {
  const fs::path& file_path = file;

  log4cpp::Category::getRoot().infoStream()
    << "Getting file detail for " << file_path;

  if (!fs::exists(file_path)) {
    throw std::runtime_error("File does not exist.");
  }

  struct stat file_stat {};

  if (stat(file_path.c_str(), &file_stat) != 0) {
    throw std::runtime_error("Failed to get file details.");
  }

  struct passwd* pwd = getpwuid(file_stat.st_uid);
  struct group* grp = getgrgid(file_stat.st_gid);

  FileDetail file_detail = {
    .type = fs::status(file_path).type(),
    .createTime = static_cast<double>(file_stat.st_ctime),
    .updateTime = static_cast<double>(file_stat.st_mtime),
    .size = file_stat.st_size,
    .owner = pwd->pw_name,
    .group = grp->gr_name,
    .mode = file_stat.st_mode,
    .path = file_path.string(),
    .name = file_path.filename().string(),
  };

  return file_detail;
}

/**
 * @brief 获取选定文件的元信息
 *
 * @param directory 指定的目录路径
 *
 * @return 返回一个FileDetail类型的数组
 *
 */
std::vector<FileDetail> getAllFileDetails(const fs::path& directory) {
  if (!fs::exists(directory) || !fs::is_directory(directory)) {
    throw std::runtime_error("Directory does not exist or is not a directory");
  }

  std::vector<FileDetail> file_details;

  for (const auto& entry : fs::directory_iterator(directory)) {
    file_details.push_back(getFileDetail(entry.path()));
  }

  return file_details;
}

}  // namespace zipfiles::server