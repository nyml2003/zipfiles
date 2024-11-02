import { PostCommitRequest, PostCommitResponse } from '../PostCommit';
import { backups, cachedCommitList } from './utils';
import { mock as GetFileDetail } from './GetFileDetail';
import { findLongestCommonPrefix } from '@/utils';
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
  const lca = findLongestCommonPrefix(request.files);
  backups.set(
    request.uuid,
    request.files.map(path => {
      const [name, ...rest] = path.split('/').reverse();
      return GetFileDetail({ path: rest.reverse().join('/'), name });
    }),
  );
  return cachedCommitList;
}
