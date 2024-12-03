#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
  fs::path cur_path = fs::current_path();

  std::cout << cur_path / "dir1" / "dir2" << std::endl;

  fs::create_directories(cur_path / "dir1" / "dir2");
  fs::remove_all(cur_path / "dir1" / "dir2");

  std::cout << fs::is_directory(cur_path / "dir1") << std::endl;

  return 0;
}  // namespace std::filesystemintmain()