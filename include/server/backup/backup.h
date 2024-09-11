#ifndef ZIPFILES_BACKUP_H
#define ZIPFILES_BACKUP_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::backup {
void backup(const fs::path& source, const fs::path& destination);
void backup_file(const fs::path& source, const fs::path& destination);
void backup_directory(const fs::path& source, const fs::path& destination);
void restore(const fs::path& source, const fs::path& destination);
void restore_file(const fs::path& source, const fs::path& destination);
void restore_directory(const fs::path& source, const fs::path& destination);
}  // namespace zipfiles::backup
#endif  // !ZIPFILES_BACKUP_H