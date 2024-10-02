#ifndef ZIPFILES_UTILS_H
#define ZIPFILES_UTILS_H

#include <cstdint>
#include <string>

namespace zipfiles {
int add(int, int);
void set_ifile(const std::string& filename);
void set_ofile(const std::string& filename);
void close_ifile();
void close_ofile();

// Return number of read.
int read_buf(uint8_t* buf, uint32_t size);

void write_buf(uint8_t* buf, uint32_t size);

}  // namespace zipfiles
#endif  // !ZIPFILES_UTILS_H
