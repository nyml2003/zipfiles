#include <exception>
#include <mutex>
#include <stdexcept>
#include "json/reader.h"
#include "server/configure/configure.h"
#include "server/tools/committable.h"

namespace zipfiles::server {

CommitTable::CommitTable() = default;
CommitTable::~CommitTable() = default;

/**
 * @brief 读取指定的CommitLog文件，返回一个Json::Value数组
 *
 * @param src 指定的log文件绝对路径
 *
 * @return 包含所有CommitLog的Json数组
 *
 */
void CommitTable::readCommitTable(const fs::path& src) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  // 先创建相应的目录
  // src有可能是一个相对目录或者根目录，此时是没有parent_path的
  // 要求src是一个绝对路径
  if (src.has_parent_path()) {
    fs::create_directories(src.parent_path());
  }

  // 先打开文件输出流，因为可能目标不存在，那么此时会创造一个新文件
  std::ofstream logFileNew(src, std::ios::app);
  logFileNew.close();

  // 再以输入流打开文件
  std::ifstream logFile(src);
  if (!logFile.is_open()) {
    throw std::runtime_error("Unable to open table file");
  }

  // 检查文件是否为空
  if (logFile.peek() == std::ifstream::traits_type::eof()) {
    // 空文件错误
    logFile.close();

    // 创建一个新的Json对象
    Json::Value root;

    std::ofstream logFileWrite(src);
    logFileWrite << root.toStyledString();
    logFileWrite.close();

    // 重新打开文件
    logFile.open(src);
    if (!logFile.is_open()) {
      throw std::runtime_error("Unable to open table file");
    }
  }

  Json::CharReaderBuilder readerBuilder;
  std::string errs;

  if (!Json::parseFromStream(
        readerBuilder, logFile, &getInstance().commitTable, &errs
      )) {
    // 若出错，说明文件是错误编码的
    throw std::runtime_error("Failed to parse JSON: " + errs);
  }

  logFile.close();
}

/**
 * @brief 返回一个CommitTable的视图
 *
 */
Json::Value CommitTable::readCommitTableView(const fs::path& src) {
  // 先创建相应的目录
  // src有可能是一个相对目录或者根目录，此时是没有parent_path的
  // 要求src是一个绝对路径
  if (src.has_parent_path()) {
    fs::create_directories(src.parent_path());
  }

  // 先打开文件输出流，因为可能目标不存在，那么此时会创造一个新文件
  std::ofstream logFileNew(src, std::ios::app);
  logFileNew.close();

  // 再以输入流打开文件
  std::ifstream logFile(src);
  if (!logFile.is_open()) {
    throw std::runtime_error("Unable to open table file");
  }

  // 检查文件是否为空
  if (logFile.peek() == std::ifstream::traits_type::eof()) {
    // 空文件错误
    logFile.close();

    // 创建一个新的Json对象
    Json::Value root;

    std::ofstream logFileWrite(src);
    logFileWrite << root.toStyledString();
    logFileWrite.close();

    // 重新打开文件
    logFile.open(src);
    if (!logFile.is_open()) {
      throw std::runtime_error("Unable to open table file");
    }
  }

  Json::Value json;
  Json::CharReaderBuilder readerBuilder;
  std::string errs;

  if (!Json::parseFromStream(readerBuilder, logFile, &json, &errs)) {
    // 若出错，说明文件是错误编码的
    throw std::runtime_error("Failed to parse JSON: " + errs);
  }

  logFile.close();
  return json;
}

/**
 * @brief 判断一个CommitRecord是否已经被提交
 *
 * @param cr 指定的CommitRecord对象
 *
 */
bool CommitTable::isCommitted(const CommitTableRecord& cr) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  Json::Value& ct = getInstance().commitTable;

  if (ct.isMember(cr.uuid)) {
    return true;
  }

  // 如果没有commit，那么将当前的record先加入到CommitTable
  appendCommitRecord(cr);

  return false;
}

/**
 * @brief 在CommitTable末尾添加一个CommitRecord
 *
 * @param cr 指定的CommitRecord对象
 *
 * ! append一定要在有锁的上下文中使用
 */
void CommitTable::appendCommitRecord(const CommitTableRecord& cr) {
  Json::Value& ct = getInstance().commitTable;

  if (!ct.isMember(cr.uuid)) {
    ct[cr.uuid] = toJson(cr);
  }
}

/**
 * @brief 给定一个路径，将CommitTable文件写入(json形式)
 *
 * @param dst 指定的table文件路径
 *
 */
void CommitTable::writeCommitTable(const fs::path& dst) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  // 写回文件
  std::ofstream tableFileWrite(dst, std::ios::binary | std::ios::trunc);
  tableFileWrite << getInstance().commitTable.toStyledString();
  tableFileWrite.close();
}

/**
 * @brief 给定uuid，删除指定的record(不写硬盘)
 *
 * @param uuid 给定的uuid
 *
 */
void CommitTable::deleteCommitRecord(const std::string& uuid) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  Json::Value& ct = getInstance().commitTable;

  if (ct.isMember(uuid)) {
    if (ct[uuid].isMember("isDelete")) {
      try {
        ct[uuid]["isDelete"] = true;
        return;
      } catch (std::exception& e) {
        throw std::runtime_error(
          "Cannot set commit record by given uuid " + uuid
        );
      }
    }
  }

  // 找不到对应的commit record
  throw std::runtime_error(
    "Cannot delete specific commit record by given uuid " + uuid
  );
}

/**
 * @brief 给定uuid，删除指定的record(不写硬盘)
 *
 * @param uuid 给定的uuid
 *
 */
void CommitTable::removeCommitRecord(const std::string& uuid) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  Json::Value& ct = getInstance().commitTable;

  if (ct.isMember(uuid)) {
    try {
      ct.removeMember(uuid);
      return;
    } catch (std::exception& e) {
      throw std::runtime_error(
        "Cannot set commit record by given uuid " + uuid
      );
    }
  }

  // 找不到对应的commit record
  throw std::runtime_error(
    "Cannot remove specific commit record by given uuid " + uuid
  );
}

/**
 * @brief 给定uuid，返回指定的CommitRecord
 *
 * @param uuid 给定的uuid
 *
 */
CommitTableRecord CommitTable::getCommitRecordById(const std::string& uuid) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  Json::Value& ct = getInstance().commitTable;

  if (ct.isMember(uuid)) {
    return fromJson(ct[uuid]);
  }

  // 找不到对应的commit record
  throw std::runtime_error(
    "Cannot find specific commit record by given uuid " + uuid
  );
}

/**
 * @brief 给定uuid，从硬盘中读取某个CommitRecord
 *
 */
CommitTableRecord CommitTable::getCommitRecordViewById(const std::string& uuid
) {
  Json::Value ct = readCommitTableView(COMMIT_TABLE_PATH);

  if (ct.isMember(uuid)) {
    CommitTableRecord cr;
    Json::Value json = ct[uuid];

    cr.uuid = uuid;
    cr.message = json["message"].asString();
    cr.createTime = json["createTime"].asDouble();
    cr.storagePath = json["storagePath"].asString();
    cr.author = json["author"].asString();
    cr.isEncrypt = json["isEncrypt"].asBool();
    cr.isDelete = json["isDelete"].asBool();

    return cr;
  }

  throw std::runtime_error(
    "Cannot find commit record view by given uuid " + uuid
  );
}

/**
 * @brief 给定CommitRecord，返回其Json(去除uuid)
 *
 * @param cr 给定的CommitRecord
 *
 */
Json::Value CommitTable::toJson(const CommitTableRecord& cr) {
  Json::Value json;

  json["message"] = cr.message;
  json["createTime"] = cr.createTime;
  json["storagePath"] = cr.storagePath;
  json["author"] = cr.author;
  json["isEncrypt"] = cr.isEncrypt;
  json["isDelete"] = cr.isDelete;
  json["encodedKey"] = cr.encodedKey;

  return json;
}

/**
 * @brief 给定Json，返回其CommitRecord(去除uuid)
 *
 * @param json 给定的Json
 *
 */
CommitTableRecord CommitTable::fromJson(Json::Value& json) {
  CommitTableRecord cr;

  if (!json.isMember("message") || !json.isMember("createTime")||!json.isMember("storagePath") || !json.isMember("author") || !json.isMember("isEncrypt") || !json.isMember("isDelete")) {
    throw std::runtime_error("Illegal JSON format");
  }

  cr.message = json["message"].asString();
  cr.createTime = json["createTime"].asDouble();
  cr.storagePath = json["storagePath"].asString();
  cr.author = json["author"].asString();
  cr.isEncrypt = json["isEncrypt"].asBool();
  cr.isDelete = json["isDelete"].asBool();
  cr.encodedKey = json["encodedKey"].asString();

  return cr;
}

}  // namespace zipfiles::server
