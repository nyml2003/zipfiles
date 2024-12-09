import { FileType } from "@/types";
import { CommitLog, FileDetail, RootFileDetail } from "./types";
import { MockFileDepth, MockFileNumber } from "./const";
import {
  generateRandomFileDetail,
  generateWholeFileTree,
  mockPostCommit,
  randomNumber,
  pickFiles
} from "./utils";
import Mock from "mockjs";

export const cachedFileRoot: RootFileDetail = {
  name: "",
  type: FileType.Directory,
  children: Array.from({ length: randomNumber(...MockFileNumber) }).map(() =>
    generateRandomFileDetail("")
  )
};

cachedFileRoot.children.push({
  name: "usr",
  type: FileType.Directory,
  children: Array.from({ length: randomNumber(...MockFileNumber) }).map(() =>
    generateRandomFileDetail("usr")
  ),
  createTime: new Date(Mock.mock("@datetime")).getTime() / 1000,
  updateTime: new Date(Mock.mock("@datetime")).getTime() / 1000,
  size: 0,
  owner: Mock.mock("@name"),
  group: Mock.mock("@name"),
  mode: Mock.mock("@integer(0, 777)"),
  path: ""
});

generateWholeFileTree(cachedFileRoot.children, MockFileDepth);

// 存储uuid和文件列表的映射
export const backups: Map<string, FileDetail[]> = new Map();

// 存储备份列表
export const cachedCommitList: CommitLog[] = [];

// 初始化commit log
Array.from({ length: 2 }).forEach(() =>
  mockPostCommit(pickFiles(cachedFileRoot.children).map(file => file.path + "/" + file.name), cachedCommitList, backups, cachedFileRoot)
);

export const config = {
  defaultBackupPath: "/usr"
};
