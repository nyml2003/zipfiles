import { GetCommitDetailRequest, GetCommitDetailResponse } from './GetCommitDetail';
import { GetCommitListRequest, GetCommitListResponse } from './GetCommitList';
import { GetFileDetailRequest, GetFileDetailResponse } from './GetFileDetail';
import { GetFileDetailListRequest, GetFileDetailListResponse } from './GetFileDetailList';
import { GetFileListRequest, GetFileListResponse } from './GetFileList';
import { PostCommitRequest, PostCommitResponse } from './PostCommit';

export enum ApiEnum {
  ERROR = 0,
  Log = 'log',
  Error = 'error',
  Warn = 'warn',
  Info = 'info',
  GetFileList = 100,
  GetFileDetailList = 101,
  PostCommit = 102,
  GetCommitDetail = 103,
  GetCommitList = 104,
  GetFileDetail = 105,
}
