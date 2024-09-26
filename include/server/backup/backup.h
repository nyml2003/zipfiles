#ifndef ZIPFILES_SERVER_BACKUP_H
#define ZIPFILES_SERVER_BACKUP_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
/**
 * @brief 备份参数
 * @details 写成结构体是为了避免传参失误
 * source: 源文件或目录
 * destination: 目标文件或目录
 */
struct BackupParams {
  fs::path source;
  fs::path destination;
};
/**
 * @brief 备份
 * @param params 备份参数
 */
void backup(const BackupParams& params);
/**
 * @brief 备份文件
 * @param params 备份参数
 */
void backup_file(const BackupParams& params);
/**
 * @brief 备份目录
 * @param params 备份参数
 */
void backup_directory(const BackupParams& params);
/**
 * @brief 恢复
 * @param params 备份参数
 */
void restore(const BackupParams& params);
/**
 * @brief 恢复文件
 * @param params 备份参数
 */
void restore_file(const BackupParams& params);
/**
 * @brief 恢复目录
 * @param params 备份参数
 */
void restore_directory(const BackupParams& params);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_BACKUP_H