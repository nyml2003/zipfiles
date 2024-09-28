export enum ApiEnum {
  Sum = 'sum',
  GetVersion = 'getVersion',
  Log = 'log',
  GetFileList = 1,
  PostBackup = 'postBackup',
  GetFileDetail = 0,
}
import { mock as GetFileList } from '@/apis/GetFileList';
import { mock as GetFileDetail } from '@/apis/GetFileDetail';
export const mockApi: { [key in ApiEnum]: Function } = {
  [ApiEnum.GetFileList]: GetFileList,
  [ApiEnum.Sum]: () => 0,
  [ApiEnum.GetVersion]: () => '1.0.0',
  [ApiEnum.Log]: () => {},
  [ApiEnum.PostBackup]: () => { },
  [ApiEnum.GetFileDetail]: GetFileDetail,
};
