/* eslint-disable @typescript-eslint/no-empty-function */
import { mock as GetFileList } from "./GetFileList";
import { mock as GetFileDetailList } from "./GetFileDetailList";
import { mock as GetCommitList } from "./GetCommitList";
import { mock as PostCommit } from "./PostCommit";
import { mock as GetCommitDetail } from "./GetCommitDetail";
import { mock as GetFileDetail } from "./GetFileDetail";
import { mock as Restore } from "./Restore";
import { mock as LogicDeleteCommit } from "./LogicDeleteCommit";
import { mock as PhysicalDeleteCommit } from "./PhysicalDeleteCommit";
import { mock as GetCommitRecycleBin } from "./GetCommitRecycleBin";
import { mock as UpdateConfig } from "./UpdateConfig";
import { mock as ReadConfig } from "./ReadConfig";
import { mock as RecoverCommit } from "./RecoverCommit";
import { mock as CreateNullFolder } from "./CreateNullFolder";
import { ApiEnum } from "..";
// eslint-disable-next-line @typescript-eslint/ban-types, @typescript-eslint/no-explicit-any
export const mockApi: Record<ApiEnum, (req: any) => any> = {
  [ApiEnum.GetFileList]: GetFileList,
  [ApiEnum.Log]: () => {},
  [ApiEnum.ERROR]: () => {
    alert("api error");
  },
  [ApiEnum.Error]: () => {},
  [ApiEnum.GetFileDetailList]: GetFileDetailList,
  [ApiEnum.PostCommit]: PostCommit,
  [ApiEnum.Warn]: () => {},
  [ApiEnum.Info]: () => {},
  [ApiEnum.GetCommitList]: GetCommitList,
  [ApiEnum.GetCommitDetail]: GetCommitDetail,
  [ApiEnum.GetFileDetail]: GetFileDetail,
  [ApiEnum.MOCK_MANY_NOTIFICATIONS]: () => {},
  [ApiEnum.Restore]: Restore,
  [ApiEnum.LogicDeleteCommit]: LogicDeleteCommit,
  [ApiEnum.PhysicalDeleteCommit]: PhysicalDeleteCommit,
  [ApiEnum.UpdateConfig]: UpdateConfig,
  [ApiEnum.GetCommitRecycleBin]: GetCommitRecycleBin,
  [ApiEnum.ReadConfig]: ReadConfig,
  [ApiEnum.RecoverCommit]: RecoverCommit,
  [ApiEnum.Connect]: () => { },
  [ApiEnum.CreateNullFolder]: CreateNullFolder
};
