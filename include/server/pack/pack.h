#ifndef ZIPFILES_SERVER_PACK_H
#define ZIPFILES_SERVER_PACK_H

#include <filesystem>
#include <fstream>

namespace zipfiles::server {
namespace fs = std::filesystem;
void packFileToArchive(std::ofstream& archive, const fs::path& filePath);
std::ofstream createArchive(const std::string& archiveName);
}  // namespace zipfiles::server
#endif  // !ZIPFILES_PACK_H