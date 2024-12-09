import { PostCommitRequest, PostCommitResponse } from "../PostCommit";
import { saveFiles } from "./utils";
import { backups, cachedCommitList, cachedFileRoot } from "./init";
export function mock(request: PostCommitRequest): PostCommitResponse {
  cachedCommitList.push({
    uuid: request.uuid,
    message: request.message,
    createTime: request.createTime,
    storagePath: "/usr/local/zipfiles",
    isEncrypt: request.isEncrypt,
    isDelete: false,
    author: request.author
  });
  saveFiles(request.files, request.uuid, backups, cachedFileRoot);
  return {};
}
