
#include "copyfile.h"
#include <fstream>
#include <iostream>
namespace zipfiles {
void copyfile(const std::string& src_filename,
              const std::string& dst_filename) {
    std::ifstream src_file(src_filename);
    if (!src_file)
        return;
    std::ofstream dst_file(dst_filename, std::ios::binary | std::ios::trunc);
    if (!dst_file)
        return;
    char buffer;
    while (true) {
        src_file.read(&buffer, sizeof(buffer));
        if (src_file.eof())
            break;
        dst_file.write(&buffer, sizeof(buffer));
    }
    src_file.close();
    dst_file.close();
}
}  // namespace zipfiles
