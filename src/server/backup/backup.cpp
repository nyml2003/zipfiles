#include <exception>
#include <filesystem>
#include <fstream>
#include <log4cpp/Category.hh>
#include <stdexcept>
#include <vector>
#include "json/value.h"
#include "json/writer.h"
#include "mp/dto.h"
#include "server/backup/backup.h"
#include "server/pack/pack.h"
#include "server/restore/restore.h"

namespace zipfiles::server {

/**
 * @brief 前端的直接调用接口，处理整个commit流程
 *
 * @param files 一个文件路径数组
 *
 * todo: 将CommitLog替换为Json value形式
 */
void backupFiles(const std::vector<fs::path>& files, const Json::Value& cl) {
  log4cpp::Category::getRoot().infoStream()
    << "Backup started, log messeag is \"" << cl["message"].asString()
    << "\" at " << cl["createTime"].asString();

  fs::path src = "~/.zip";

  Json::Value cls = readCommitLog(src);

  if (isCommitted(cls, cl)) {
    throw std::runtime_error(
      "Commit log is already committed, its uuid is " + cl["uuid"].asString()
    );
  }

  // 打包
  try {
    std::ofstream archive = createArchive(cl["storagePath"].asString());
    for (const auto& file : files) {
      packFileToArchive(archive, file);
    }
  } catch (const std::exception& e) {
    throw std::runtime_error(
      "Error occurred when trying to pack, its uuid is " +
      cl["uuid"].asString() + ", because " + e.what()
    );
  }

  try {
    appendCommitLog(cls, cl);
  } catch (const std::exception& e) {
    throw std::runtime_error(
      "Error occurred when trying to append commit log, its uuid is " +
      cl["uuid"].asString() + ", because " + e.what()
    );
  }

  // todo: 压缩
  // todo: 加密
}

/**
 * @brief 判断一个CommitLog是否已经被提交
 *
 * @param src 指定的CommitLog文件，以json的形式展示
 *
 * @param cl 指定的CommitLog对象
 *
 */
bool isCommitted(Json::Value& cls, const Json::Value& cl) {
  if (!(cls.isMember("data") && cls["data"].isArray())) {
    throw std::runtime_error("Illegal Json format");
  }

  for (const auto& log : cls["data"]) {
    // 检查每个元素是否包含"uuid"字段且和目标cl匹配
    if (log.isMember("uuid") || log["uuid"].asString() == cl["uuid"].asString()) {
      return true;
    }
  }

  return false;
}

/**
 * @brief 给定已有的commitlog文件(json形式)，在末尾添加一个CommitLog
 *
 * @param dst 指定的log文件路径
 *
 * @param cl 指定的CommitLog对象
 *
 */
void appendCommitLog(Json::Value& dst, const Json::Value& cl) {
  if (dst.isMember("data") && dst["data"].isArray()) {
    dst["data"].append(cl);
  } else {
    throw std::runtime_error("Illegal Json format");
  }
}

void writeCommitLog(const fs::path& dst, const Json::Value& cls) {
  // 写回文件
  std::ofstream logFileWrite(dst, std::ios::binary);
  logFileWrite << cls.toStyledString();
  logFileWrite.close();
}

/**
 * @brief 依据传进的路径数组生成一个目录树
 *
 * @param files 一个文件路径数组
 */
DirectoryTreeNode generateDirectoryTree(const std::vector<fs::path>& files) {
  // 插入根目录
  DirectoryTreeNode root;
  root.name = "/";
  root.type = fs::file_type::directory;

  for (const auto& file : files) {
    DirectoryTreeNode* cur = &root;

    for (const auto& part : file) {
      std::string partStr = part.string();

      if (cur->children.find(partStr) == cur->children.end()) {
        // 没有找到已经记录的子节点
        DirectoryTreeNode newNode;
        newNode.name = partStr;
        newNode.type = fs::symlink_status(file).type();
        cur->children[partStr] = newNode;
      }

      // 这个节点已经记录了一个目录信息，继续深入
      cur = &cur->children[partStr];
    }
  }

  return root;
}

/**
 * @brief 给定一个路径，生成一个描述Commit内容的目录树文件
 *
 * @param dst 指定的目录文件路径
 *
 * @param root 用以创建目录文件的目录树根目录
 */
void writeDirectoryFile(
  const fs::path& /*dst*/,
  const DirectoryTreeNode& /*root*/
) {}

}  // namespace zipfiles::server