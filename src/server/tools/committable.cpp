#include "server/tools/committable.h"
#include <mutex>
#include <stdexcept>
#include "json/reader.h"

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
    root["data"] = Json::arrayValue;

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
 * @brief 判断一个CommitRecord是否已经被提交
 *
 * @param cr 指定的CommitRecord对象
 *
 */
bool CommitTable::isCommitted(const Json::Value& cr) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  Json::Value& ct = getInstance().commitTable;

  if (!(ct.isMember("data") && ct["data"].isArray())) {
    throw std::runtime_error("Illegal JSON format");
  }

  for (const auto& record : ct["data"]) {
    // 检查每个元素是否包含"uuid"字段且和目标uuid匹配
    if (record.isMember("uuid") && record["uuid"].asString() == cr["uuid"].asString()) {
      return true;
    }
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
void CommitTable::appendCommitRecord(const Json::Value& cr) {
  Json::Value& ct = getInstance().commitTable;

  if (ct.isMember("data") && ct["data"].isArray()) {
    ct["data"].append(cr);
  } else {
    throw std::runtime_error("Illegal JSON format");
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
  std::ofstream logFileWrite(dst, std::ios::binary | std::ios::trunc);
  logFileWrite << getInstance().commitTable.toStyledString();
  logFileWrite.close();
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

  if (!(ct.isMember("data") && ct["data"].isArray())) {
    throw std::runtime_error("Illegal JSON format");
  }

  for (auto& record : ct["data"]) {
    // 检查每个元素是否包含"uuid"字段且和目标uuid匹配
    if (record.isMember("uuid") && record["uuid"].asString() == uuid) {
      if (record.isMember("isDelete")) {
        record["isDelete"] = true;
        return;
      }

      throw std::runtime_error("Illegal Record format");
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

  if (!(ct.isMember("data") && ct["data"].isArray())) {
    throw std::runtime_error("Illegal JSON format");
  }

  Json::Value& array = ct["data"];

  // 检查每个元素是否包含"uuid"字段且和目标uuid匹配
  for (unsigned int i = 0; i < array.size(); ++i) {
    if (array[i].isMember("uuid") && array[i]["uuid"].asString() == uuid) {
      array.removeIndex(i, nullptr);
      break;
    }
  }
}

/**
 * @brief 给定uuid，返回指定的CommitRecord
 *
 * @param uuid 给定的uuid
 *
 */
Json::Value CommitTable::getCommitRecordById(const std::string& uuid) {
  // 获取锁
  std::lock_guard<std::mutex> lock(getInstance().mutex);

  Json::Value& ct = getInstance().commitTable;

  if (!(ct.isMember("data") && ct["data"].isArray())) {
    throw std::runtime_error("Illegal JSON format");
  }

  for (const auto& record : ct["data"]) {
    // 检查每个元素是否包含"uuid"字段且和目标uuid匹配
    if (record.isMember("uuid") && record["uuid"].asString() == uuid) {
      return record;
    }
  }

  // 找不到对应的commit record
  throw std::runtime_error(
    "Cannot find specific commit record by given uuid " + uuid
  );
}

}  // namespace zipfiles::server
