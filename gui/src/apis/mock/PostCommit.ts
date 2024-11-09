import { PostCommitRequest, PostCommitResponse } from '../PostCommit';
import { mock as GetFileDetail } from './GetFileDetail';
import { findLongestCommonPrefix } from '@/utils';
import { saveFiles } from './utils';
import { backups, cachedCommitList, cachedFileRoot } from './init';
import { useDispatch } from 'react-redux';
import { finishMessage } from '@/stores/NotificationReducer';
import { Code } from '@/hooks/useApi/types';
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
  saveFiles(request.files, request.uuid, backups, cachedFileRoot);
  return {};
}
