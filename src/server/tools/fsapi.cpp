#include "server/tools/fsapi.h"
#include "common.h"
#include <pwd.h>
#include <grp.h>
#include <vector>
#include "mp/dto.h"
#include "utils.h"
#include <sys/stat.h>
namespace zipfiles::server {
/**
 * @brief 获取给定目录下的所有文件和目录(No follow，只返回一层的结果)
 *
 * @param directory 指定的目录路径
 *
 * @return 返回一个File类型的数组
 *
 */
std::vector<File> getFileList(const fs::path& directory) {
  const fs::path directory_path = "/" / directory;
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

    files.push_back(file);
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
  const fs::path file_path = "/" / file;
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
    .createTime = toIso8601(file_stat.st_ctime),
    .updateTime = toIso8601(file_stat.st_mtime),
    .size = file_stat.st_size,
    .owner = pwd->pw_name,
    .group = grp->gr_name,
    .mode = file_stat.st_mode,
    .path = file_path.string(),
    .name = file_path.filename().string(),
  };

  return file_detail;
}
}  // namespace zipfiles::server