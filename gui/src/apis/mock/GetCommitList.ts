import { CommitLog, GetCommitListResponse } from '../GetCommitList';
import Mock from 'mockjs';

function generateRandomCommitLog(): CommitLog {
  return {
    uuid: Mock.mock('@guid'),
    message: Mock.mock('@sentence'),
    createTime: Mock.mock('@datetime'),
    defaultPath: Mock.mock('@url'),
    storagePath: Mock.mock('@url'),
    isEncrypt: Mock.mock('@boolean'),
    isDelete: Mock.mock('@boolean'),
    author: Mock.mock('@name'),
  };
}

export function mock(): GetCommitListResponse {
  return {
    data: Array.from({ length: 100 }, () => generateRandomCommitLog()),
  };
}
