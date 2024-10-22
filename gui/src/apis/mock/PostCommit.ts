import { PostCommitRequest, PostCommitResponse } from '../PostCommit';
import { cachedCommitList } from './utils';

export function mock(request: PostCommitRequest): PostCommitResponse {
  cachedCommitList.push({
    uuid: request.uuid,
    message: request.message,
    createTime: request.createTime,
    storagePath: '/usr/local/zipfiles',
    isEncrypt: request.isEncrypt,
    isDelete: false,
    author: request.author,
  });
  console.log(cachedCommitList);
  return {};
}
