// #include <mp/Request.h>
// #include <mp/Response.h>
// #include <server/socket/api.h>
// #include <log4cpp/Category.hh>
// namespace zipfiles::server::api {
// namespace fs = std::filesystem;
// template <>
// response::GetFileList handle<request::GetFileList, response::GetFileList>(
//   const request::GetFileList& request
// ) {
//   const fs::path& path = request.path;

//   if (!fs::exists(path)) {
//     throw std::runtime_error("File does not exist");
//   }

//   if (!fs::is_directory(path)) {
//     throw std::runtime_error("Path is not a directory");
//   }

//   response::GetFileList response;
//   for (const auto& entry : fs::directory_iterator(path)) {
//     response.files.push_back(
//       {.type = entry.status().type(), .name = entry.path().filename().string()}
//     );
//   }
//   return response;
// }

// }  // namespace zipfiles::server::api