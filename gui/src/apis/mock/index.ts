/* eslint-disable @typescript-eslint/no-empty-function */
import { mock as GetFileList } from './GetFileList';
import { mock as GetFileDetail } from './GetFileDetail';
import { mock as GetAllFileDetails } from './GetAllFileDetails';
import { mock as GetCommitList } from './GetCommitList';
import { mock as PostCommit } from './PostCommit';
import { ApiEnum } from '..';
// eslint-disable-next-line @typescript-eslint/ban-types
export const mockApi: Record<ApiEnum, Function> = {
  [ApiEnum.GetFileList]: GetFileList,
  [ApiEnum.Log]: () => {},
  [ApiEnum.UNKNOWN]: () => {},
  [ApiEnum.Error]: () => {},
  [ApiEnum.GetFileDetail]: GetFileDetail,
  [ApiEnum.PostCommit]: PostCommit,
  [ApiEnum.GetAllFileDetails]: GetAllFileDetails,
  [ApiEnum.Warn]: () => {},
  [ApiEnum.Info]: () => {},
  [ApiEnum.GetCommitList]: GetCommitList,
};
