import { mock as GetFileList } from './GetFileList';
import { mock as GetFileDetail } from './GetFileDetail';
import { ApiEnum } from '..';
export const mockApi: { [key in ApiEnum]: Function } = {
  [ApiEnum.GetFileList]: GetFileList,
  [ApiEnum.Sum]: () => 0,
  [ApiEnum.GetVersion]: () => '1.0.0',
  [ApiEnum.Log]: () => {},
  [ApiEnum.PostBackup]: () => {},
  [ApiEnum.GetFileDetail]: GetFileDetail,
};
