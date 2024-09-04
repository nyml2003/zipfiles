#include <string>
#ifndef ZIPFILES_COPYFILE_H
#define ZIPFILES_COPYFILE_H
namespace zipfiles
{
void copyfile(const std::string &src_filename, const std::string &dst_filename);
} // namespace zipfiles
#endif // !ZIPFILES_COPYFILE_H