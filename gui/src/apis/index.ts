/* eslint-disable no-unused-vars */
/**
 * @brief: 定义所有的接口枚举
 * 数字为1xx的为Remote Function
 * 数字为9x的为Mock Function
 * 字符串为 Procedure
 */
export enum ApiEnum {
  ERROR = 0,
  Log = "log",
  Error = "error",
  Warn = "warn",
  Info = "info",
  MOCK_MANY_NOTIFICATIONS = 98,
  GetFileList = 100,
  GetFileDetailList = 101,
  PostCommit = 102,
  GetCommitDetail = 103,
  GetCommitList = 104,
  GetFileDetail = 105,
  Restore = 106,
  LogicDeleteCommit = 107,
  PhysicalDeleteCommit = 108,
  GetCommitRecycleBin = 109,
  RecoverCommit = 110,
  UpdateConfig = "updateConfig",
  ReadConfig = "readConfig",
}
