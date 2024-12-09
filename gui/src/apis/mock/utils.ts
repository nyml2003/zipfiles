/* eslint-disable prefer-const */
import { FileType } from "@/types";
import { cleanObject, findLongestCommonPrefix } from "@/utils";
import Mock from "mockjs";
import { v4 as uuidv4 } from "uuid";
import { MockFileNumber } from "./const";
import { omit } from "lodash";
import { FileDetail, MockFileDetail, Filter, CommitLog, RootFileDetail } from "./types";

export function pickIndex<T>(choices: T[], _weights?: number[]): number {
  if (!_weights) {
    return Math.floor(Math.random() * choices.length);
  }
  const weights = _weights;
  const x = Math.floor(Math.random() * weights.reduce((a, b) => a + b));
  const pre = weights.reduce((acc, cur, i) => {
    acc.push(i === 0 ? cur : acc[i - 1] + cur);
    return acc;
  }, [] as number[]);
  const binarySearch = (x: number) => {
    let low = 0,
      high = pre.length - 1;
    while (low < high) {
      const mid = Math.floor((high - low) / 2) + low;
      if (pre[mid] < x) {
        low = mid + 1;
      } else {
        high = mid;
      }
    }
    return low;
  };
  return binarySearch(x);
}
export const filterBy = <T extends FileDetail>(files: T[], filter?: Filter): T[] => {
  const cleanedFilter: Filter = cleanObject(filter);
  if (!cleanedFilter || Object.keys(cleanedFilter).length === 0) return files;
  return files.filter(file => {
    // if (file.type === FileType.Directory) return true;
    if (cleanedFilter.type && file.type !== cleanedFilter.type) return false;
    if (cleanedFilter.minSize && file.size < cleanedFilter.minSize) return false;
    if (cleanedFilter.maxSize && file.size > cleanedFilter.maxSize) return false;
    if (cleanedFilter.minCreateTime && file.createTime < cleanedFilter.minCreateTime) return false;
    if (cleanedFilter.maxCreateTime && file.createTime > cleanedFilter.maxCreateTime) return false;
    if (cleanedFilter.minUpdateTime && file.updateTime < cleanedFilter.minUpdateTime) return false;
    if (cleanedFilter.maxUpdateTime && file.updateTime > cleanedFilter.maxUpdateTime) return false;
    if (cleanedFilter.owner && !file.owner.includes(cleanedFilter.owner)) return false;
    return !(cleanedFilter.group && !file.group.includes(cleanedFilter.group));

  });
};
export function pickFileType(): FileType {
  const fileTypes = [
    FileType.None,
    FileType.NotFound,
    FileType.Regular,
    FileType.Directory,
    FileType.Symlink,
    FileType.Block,
    FileType.Character,
    FileType.Fifo,
    FileType.Socket,
    FileType.Unknown
  ];
  const weights = [1, 1, 3, 5, 1, 1, 1, 1, 1, 1];
  return pickIndex<FileType>(fileTypes, weights);
}

export function generateWholeFileTree(root: MockFileDetail[], depth: number) {
  if (depth === 0) return;
  root.forEach(file => {
    if (file.type === FileType.Directory) {
      file.children = Array.from({ length: randomNumber(...MockFileNumber) }).map(() =>
        generateRandomFileDetail(file.path + "/" + file.name)
      );
      if (depth === 1) {
        file.children = file.children.filter(file => file.type !== FileType.Directory);
      }
      generateWholeFileTree(file.children, depth - 1);
    }
  });
}

// 随机从文件列表cachedFileList中选择若干个文件
export function pickFiles(_files: MockFileDetail[]): MockFileDetail[] {
  const fileNumber = randomNumber(1, 10);
  const flatFiles = _files.reduce((acc: MockFileDetail[], file: MockFileDetail) => {
    acc.push(file);
    if (file.children) {
      acc.push(...file.children);
    }
    return acc;
  }, []);
  for (let i = flatFiles.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [flatFiles[i], flatFiles[j]] = [flatFiles[j], flatFiles[i]]; // 交换元素
  }
  return flatFiles.slice(0, fileNumber);
}

export function mockPostCommit(
  files: string[],
  cachedCommitList: CommitLog[],
  backups: Map<string, FileDetail[]>,
  cachedFileRoot: RootFileDetail
) {
  const uuid = uuidv4();
  saveFiles(files, uuid, backups, cachedFileRoot);
  cachedCommitList.push({
    uuid,
    message: Mock.mock("@sentence"),
    createTime: new Date(Mock.mock("@datetime")).getTime() / 1000,
    storagePath: "/usr/local/zipfiles",
    isDelete: false,
    isEncrypt: Mock.mock("@boolean"),
    author: Mock.mock("@name")
  });
}

export function saveFiles(
  files: string[],
  uuid: string,
  backups: Map<string, FileDetail[]>,
  cachedFileRoot: RootFileDetail
) {
  const lca = findLongestCommonPrefix(files);
  const fileDetails = files.map(path => {
    const parts = path.split("/");
    const name = parts.pop() as string;
    const targetPath = parts.join("/");
    return findFile(cachedFileRoot, targetPath, name);
  });
  const relativeFiles = fileDetails.map(file => {
    if (!file) {
      throw new Error("File not found");
    }
    let newPath = file.path.slice(lca.length);
    if (newPath[0] === "/") {
      newPath = newPath.slice(1);
    }
    if (newPath === "") {
      newPath = ".";
    }
    return {
      ...omit(file, "children"),
      path: newPath
    };
  });
  backups.set(uuid, relativeFiles);
}

export function generateRandomFileDetail(path: string): MockFileDetail {
  const type = pickFileType(); // 随机选择文件类型
  const name = Mock.mock("@word(3, 10)");
  return {
    name,
    type,
    children: null,
    // 随机生成数值时间戳
    createTime: new Date(Mock.mock("@datetime")).getTime() / 1000,
    updateTime: new Date(Mock.mock("@datetime")).getTime() / 1000,
    size: type === FileType.Regular ? Mock.mock("@integer(1024, 1024 * 1024)") : 0,
    owner: Mock.mock("@name"),
    group: Mock.mock("@name"),
    mode: Mock.mock("@integer(0, 777)"),
    path
  };
}
export function findFilesByPath(
  cachedFileRoot: RootFileDetail,
  targetPath: string,
  filter?: Filter
): MockFileDetail[] | null {
  if (targetPath === "") {
    return filterBy(cachedFileRoot.children, filter);
  }
  const parts = targetPath.split("/").slice(1);
  let current: MockFileDetail = cachedFileRoot.children.find(
    file => file.name === parts[0]
  ) as MockFileDetail;
  for (let i = 1; i < parts.length; i++) {
    if (!current.children) {
      throw new Error("File not found" + targetPath);
    }
    current = current.children.find(file => file.name === parts[i]) as MockFileDetail;
  }
  return filterBy(current.children || [], filter);
}

export function findFile(
  cachedFileRoot: RootFileDetail,
  targetPath: string,
  targetName: string
): MockFileDetail | null {
  const files = findFilesByPath(cachedFileRoot, targetPath);
  if (!files) return null;
  return files.find(file => file.name === targetName) || null;
}

export function createNullFolder(
  cachedFileRoot: RootFileDetail,
  targetPath: string,
  newFolderName: string
) {
  if (targetPath === "") {
    const index = cachedFileRoot.children.findIndex(file => file.name === newFolderName);
    if (index !== -1) {
      throw new Error("Folder already exists");
    }
    cachedFileRoot.children.push({
      name: newFolderName,
      type: FileType.Directory,
      children: null,
      createTime: new Date().getTime() / 1000,
      updateTime: new Date().getTime() / 1000,
      size: 0,
      owner: "root",
      group: "root",
      mode: 777,
      path: "/" + newFolderName
    });
    return;
  }
  const parts = targetPath.split("/").slice(1);
  let current: MockFileDetail = cachedFileRoot.children.find(
    file => file.name === parts[0]
  ) as MockFileDetail;
  for (let i = 1; i < parts.length; i++) {
    if (!current.children) {
      break;
    }
    current = current.children.find(file => file.name === parts[i]) as MockFileDetail;
  }
  current.children = current.children || [];
  current.children.push({
    name: newFolderName,
    type: FileType.Directory,
    children: null,
    createTime: new Date().getTime() / 1000,
    updateTime: new Date().getTime() / 1000,
    size: 0,
    owner: "root",
    group: "root",
    mode: 777,
    path: targetPath + "/" + newFolderName
  });
}

export function randomNumber(min: number, max: number) {
  return Math.floor(Math.random() * (max - min + 1) + min);
}
