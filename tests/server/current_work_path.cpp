#include <filesystem>
#include <iostream>

int main() {
  try {
    // 获取当前工作目录
    std::filesystem::path current_path = std::filesystem::current_path();
    std::cout << "当前的工作目录是: " << current_path << std::endl;
    current_path = current_path.parent_path();
    std::cout << "当前的项目根目录是: " << current_path << std::endl;
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "错误: " << e.what() << std::endl;
  }

  return 0;
}