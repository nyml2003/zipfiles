#ifndef ZIPFILES_UNITTEST_CONSTANT_H
#define ZIPFILES_UNITTEST_CONSTANT_H
#include <filesystem>
namespace fs = std::filesystem;
namespace zipfiles::server::unittest {
const fs::path mockDir = "/app/tests/mock";
const fs::path backupDir = mockDir / "backup";
const fs::path userDir = mockDir / "user";
}  // namespace zipfiles::unittest
#endif  // !ZIPFILES_UNITTEST_CONSTANT_H