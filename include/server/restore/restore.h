#ifndef ZIPFILES_SERVER_RESTORE_H
#define ZIPFILES_SERVER_RESTORE_H
#include <filesystem>
#include "common.h"
namespace fs = std::filesystem;
namespace zipfiles::server {
/**
 * @brief 恢复
 * @param params 备份参数
 */
void restore(const CommitLog& params, const fs::path& dst);
/**
 * @brief 恢复文件
 * @param params 备份参数
 */
void restore_file(const CommitLog& params, const fs::path& dst);
/**
 * @brief 恢复目录
 * @param params 备份参数
 */
void restore_directory(const CommitLog& params, const fs::path& dst);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_RESTORE_H