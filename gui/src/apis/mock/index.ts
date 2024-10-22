/* eslint-disable @typescript-eslint/no-empty-function */
import { mock as GetFileList } from './GetFileList';
import { mock as GetFileDetailList } from './GetFileDetailList';
import { mock as GetCommitList } from './GetCommitList';
import { mock as PostCommit } from './PostCommit';
import { mock as GetCommitDetail } from './GetCommitDetail';
import { mock as GetFileDetail } from './GetFileDetail';
import { ApiEnum } from '..';
// eslint-disable-next-line @typescript-eslint/ban-types
export const mockApi: Record<ApiEnum, Function> = {
  [ApiEnum.GetFileList]: GetFileList,
  [ApiEnum.Log]: () => {},
  [ApiEnum.ERROR]: () => {alert('api error');},
  [ApiEnum.Error]: () => {},
  [ApiEnum.GetFileDetailList]: GetFileDetailList,
  [ApiEnum.PostCommit]: PostCommit,
  [ApiEnum.Warn]: () => {},
  [ApiEnum.Info]: () => {},
  [ApiEnum.GetCommitList]: GetCommitList,
  [ApiEnum.GetCommitDetail]: GetCommitDetail,
  [ApiEnum.GetFileDetail]: GetFileDetail,
};
