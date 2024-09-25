#ifndef ZIPFILES_SERVER_BACKUP_H
#define ZIPFILES_SERVER_BACKUP_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server {
struct BackupParams {
  fs::path source;
  fs::path destination;
};
void backup(const BackupParams& params);
void backup_file(const BackupParams& params);
void backup_directory(const BackupParams& params);
void restore(const BackupParams& params);
void restore_file(const BackupParams& params);
void restore_directory(const BackupParams& params);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_BACKUP_H