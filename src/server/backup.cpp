#include "server/backup/backup.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace zipfiles::server {

// 备份文件
void backup_file(const fs::path& source, const fs::path& destination) {
    std::ifstream ifs(source, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open source file: " +
                                 source.string());
    }
    std::ofstream ofs(destination, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Failed to open destination file: " +
                                 destination.string());
    }
    ofs << ifs.rdbuf();
}

// 备份目录
void backup_directory(const fs::path& source, const fs::path& destination) {
    if (!fs::exists(source)) {
        throw std::runtime_error("Source directory does not exist: " +
                                 source.string());
    }
    fs::create_directories(destination);
    for (const auto& entry : fs::recursive_directory_iterator(source)) {
        const auto& path = entry.path();
        auto relative_path = fs::relative(path, source);
        if (fs::is_directory(path)) {
            fs::create_directories(destination / relative_path);
        } else if (fs::is_regular_file(path)) {
            backup_file(path, destination / relative_path);
        } else {
            throw std::runtime_error(
                "Source path is neither a file nor a directory: " +
                path.string());
        }
    }
}

// 备份
void backup(const fs::path& source, const fs::path& destination) {
    if (!fs::exists(source)) {
        throw std::runtime_error("Source path does not exist: " +
                                 source.string());
    }
    if (fs::is_directory(source)) {
        backup_directory(source, destination);
    } else if (fs::is_regular_file(source)) {
        backup_file(source, destination);
    } else {
        throw std::runtime_error(
            "Source path is neither a file nor a directory: " +
            source.string());
    }
}

// 还原文件
void restore_file(const fs::path& source, const fs::path& destination) {
    std::ifstream ifs(source, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open source file: " +
                                 source.string());
    }
    std::ofstream ofs(destination, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Failed to open destination file: " +
                                 destination.string());
    }
    ofs << ifs.rdbuf();
}

// 还原目录
void restore_directory(const fs::path& source, const fs::path& destination) {
    if (!fs::exists(source)) {
        throw std::runtime_error("Source directory does not exist: " +
                                 source.string());
    }
    fs::create_directories(destination);
    for (const auto& entry : fs::recursive_directory_iterator(source)) {
        const auto& path = entry.path();
        auto relative_path = fs::relative(path, source);
        if (fs::is_directory(path)) {
            fs::create_directories(destination / relative_path);
        } else if (fs::is_regular_file(path)) {
            restore_file(path, destination / relative_path);
        } else {
            throw std::runtime_error(
                "Source path is neither a file nor a directory: " +
                path.string());
        }
    }
}

// 还原
void restore(const fs::path& source, const fs::path& destination) {
    if (!fs::exists(source)) {
        throw std::runtime_error("Source path does not exist: " +
                                 source.string());
    }
    if (fs::is_directory(source)) {
        restore_directory(source, destination);
    } else if (fs::is_regular_file(source)) {
        restore_file(source, destination);
    } else {
        throw std::runtime_error(
            "Source path is neither a file nor a directory: " +
            source.string());
    }
}

}  // namespace zipfiles::server