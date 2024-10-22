import { GetCommitListResponse } from '../GetCommitList';
import { cachedCommitList } from './utils';

// function generateRandomCommitLog(): CommitLog {
//   return {
//     uuid: Mock.mock('@guid'),
//     message: Mock.mock('@sentence'),
//     createTime: Mock.mock('@datetime'),
//     lca: Mock.mock('@url'),
//     storagePath: Mock.mock('@url'),
//     isEncrypt: Mock.mock('@boolean'),
//     isDelete: Mock.mock('@boolean'),
//     author: Mock.mock('@name'),
//   };
// }

export function mock(): GetCommitListResponse {
  return {
    commits: cachedCommitList,
  };
}
