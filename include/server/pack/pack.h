#ifndef ZIPFILES_SERVER_PACK_H
#define ZIPFILES_SERVER_PACK_H

#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace zipfiles::server {
/**
 * !Deprecated
 *
 */
std::ofstream createArchive(const std::string& archiveName);
void packFileToArchive(std::ofstream& archive, const fs::path& filePath);

std::vector<uint8_t> packFiles(const std::vector<fs::path>& files);

}  // namespace zipfiles::server
#endif  // !ZIPFILES_PACK_H