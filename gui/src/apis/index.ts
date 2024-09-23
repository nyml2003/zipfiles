export enum ApiEnum {
  Sum = 'sum',
  GetVersion = 'getVersion',
  Log = 'log',
  GetFileList = 'getFileList',
  PostBackup = 'postBackup',
}
import { mock as GetFileList } from '@/apis/GetFileList';
export const mockApi: { [key in ApiEnum]: Function } = {
  [ApiEnum.GetFileList]: GetFileList,
  [ApiEnum.Sum]: () => 0,
  [ApiEnum.GetVersion]: () => '1.0.0',
  [ApiEnum.Log]: () => {},
  [ApiEnum.PostBackup]: () => {},
};
