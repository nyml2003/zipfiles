import { mock as GetFileList } from './GetFileList';
import { mock as GetFileDetail } from './GetFileDetail';
import { mock as GetAllFileDetails } from './GetAllFileDetails';
import { ApiEnum } from '..';
export const mockApi: { [key in ApiEnum]: Function } = {
  [ApiEnum.GetFileList]: GetFileList,
  [ApiEnum.Log]: () => { },
  [ApiEnum.UNKNOWN]: () => { },
  [ApiEnum.Error]: () => { },
  [ApiEnum.GetFileDetail]: GetFileDetail,
  [ApiEnum.PostCommit]: () => { },
  [ApiEnum.GetAllFileDetails]: GetAllFileDetails,
};
