// #ifndef ZIPFILES_MP_RESPONSE_H
// #define ZIPFILES_MP_RESPONSE_H
// #include <json/json.h>
// #include <variant>
// #include "mp/dto.h"

// namespace zipfiles {

// enum class StatusCode {
//   UNKNOWN = 0,
//   OK = 1,
//   ERROR = 2,
// };

// enum class ApiEnum {
//   IGNORE = 1,
//   ERROR = 0,
//   MOCK_NEED_TIME = 99,
//   GET_FILE_DETAIL = 100,
//   GET_FILE_LIST = 101,
//   POST_COMMIT = 102,
//   GET_ALL_FILE_DETAILS = 103,
// };

// size_t toSizeT(ApiEnum apiEnum);

// std::ostream& operator<<(std::ostream& os, const StatusCode& status);

// namespace response {

// struct GetFileDetail {
//   FileDetail metadata;
// };

// struct GetFileList {
//   std::vector<File> files;
// };
// struct MockNeedTime {
//   int id;
// };

// struct PostCommit {};

// struct GetAllFileDetails {
//   std::vector<FileDetail> files;
// };

// }  // namespace response

// struct Res;

// using ResKind = std::variant<
//   response::GetFileDetail,
//   response::GetFileList,
//   response::MockNeedTime,
//   response::PostCommit,
//   response::GetAllFileDetails>;

// using ResPtr = std::shared_ptr<Res>;

// struct Res {
//   explicit Res(ResKind kind);
//   ResKind kind;
//   double timestamp{};
//   std::string uuid;
//   StatusCode status{};
//   Json::Value toJson();
//   static ResPtr fromJson(const Json::Value& json);
// };

// ResPtr makeResGetFileDetail(FileDetail metadata);
// ResPtr makeResGetFileDetail(Json::Value payload);

// ResPtr makeResGetFileList(std::vector<File> files);
// ResPtr makeResGetFileList(Json::Value payload);

// ResPtr makeResMockNeedTime(int id);
// ResPtr makeResMockNeedTime(Json::Value payload);

// ResPtr makeResPostCommit();
// ResPtr makeResPostCommit(Json::Value payload);

// ResPtr makeResGetAllFileDetails(std::vector<FileDetail> metadata);
// ResPtr makeResGetAllFileDetails(Json::Value payload);

// }  // namespace zipfiles

// #endif