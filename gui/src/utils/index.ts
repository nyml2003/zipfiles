import { FileType } from "@/types";

interface FileDetail {
  name: string;
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}
interface File {
  name: string;
  type: FileType;
}

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export function cleanObject(obj: any) {
  if (obj === null || obj === undefined) return obj;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  return Object.entries(obj).reduce((acc: any, [key, value]) => {
    if (value && typeof value === "object") {
      value = cleanObject(value);
    }
    if (value !== null && value !== undefined) {
      acc[key] = value;
    }
    return acc;
  }, {});
}

export function findLongestCommonPrefix(paths: string[]) {
  if (paths.length === 0) return "";
  if (paths.length === 1) return paths[0];

  // 将路径分割成数组
  const splitPaths = paths.map(path => path.split("/"));

  // 找到最短路径的长度，因为公共前缀的长度不会超过最短路径
  const minLength = Math.min(...splitPaths.map(p => p.length));

  // 初始化最长公共前缀数组
  const commonPrefix: string[] = [];

  // 遍历每个路径元素
  for (let i = 0; i < minLength; i++) {
    const firstPathElement = splitPaths[0][i];
    // 检查当前元素是否在所有路径中都相同
    const allSame = splitPaths.every(path => path[i] === firstPathElement);
    if (allSame) {
      commonPrefix.push(firstPathElement);
    } else {
      break;
    }
  }
  // 将数组转换回字符串，并用斜杠连接
  return commonPrefix.join("/");
}
export type Dir<DataType> = {
  children: DataType[];
  subDir: Dir<DataType>[];
  name: string;
};
export function buildTree<
  DataType extends {
    path: string;
    type: FileType;
  },
>(files: DataType[]): Dir<DataType> {
  const root: Dir<DataType> = { children: [], name: "", subDir: [] };
  files.forEach(file => {
    if (file.path === ".") {
      file.path = "";
      root.children.push(file);
      return;
    }
    const parts = file.path.split("/");
    let current = root;
    for (let i = 0; i < parts.length; i++) {
      const part = parts[i];
      const childIndex = current.subDir.findIndex(subDir => subDir.name === part);
      if (childIndex === -1) {
        current.subDir.push({
          name: part,
          subDir: [],
          children: []
        });
        current = current.subDir[current.subDir.length - 1];
      } else {
        current = current.subDir[childIndex];
      }
    }
    current.children.push(file);
  });
  return root;
}

export function findFile(files: Dir<FileDetail>, targetPath: string): (FileDetail | File)[] {
  let current = files;
  if (targetPath !== "") {
    const parts = targetPath.split("/");
    for (let i = 0; i < parts.length; i++) {
      const part = parts[i];
      const child = current.subDir.find(subDir => subDir.name === part);
      if (!child) return [];
      current = child;
    }
  }
  const result: (FileDetail | File)[] = current.children;
  current.subDir.forEach(subDir => {
    if (!result.find(file => file.name === subDir.name)) {
      result.push({
        name: subDir.name,
        type: FileType.Directory
      });
    }
  });
  return result;
}

export type Required<T> = T extends unknown
  ? {
      [P in keyof T]-?: Required<T[P]>;
    }
  : never;

export function modeToString(mode: number): string {
  // 权限位常量
  const S_IRUSR = 0o400; // 用户（所有者）权限: 读
  const S_IWUSR = 0o200; // 用户（所有者）权限: 写
  const S_IXUSR = 0o100; // 用户（所有者）权限: 执行

  const S_IRGRP = 0o040; // 用户组权限: 读
  const S_IWGRP = 0o020; // 用户组权限: 写
  const S_IXGRP = 0o010; // 用户组权限: 执行

  const S_IROTH = 0o004; // 其他人权限: 读
  const S_IWOTH = 0o002; // 其他人权限: 写
  const S_IXOTH = 0o001; // 其他人权限: 执行

  // 检查并构建权限字符串
  let permissions = "";

  // 用户权限
  if ((mode & S_IRUSR) === S_IRUSR) permissions += "r";
  else permissions += "-";
  if ((mode & S_IWUSR) === S_IWUSR) permissions += "w";
  else permissions += "-";
  if ((mode & S_IXUSR) === S_IXUSR) permissions += "x";
  else permissions += "-";

  // 用户组权限
  if ((mode & S_IRGRP) === S_IRGRP) permissions += "r";
  else permissions += "-";
  if ((mode & S_IWGRP) === S_IWGRP) permissions += "w";
  else permissions += "-";
  if ((mode & S_IXGRP) === S_IXGRP) permissions += "x";
  else permissions += "-";

  // 其他人权限
  if ((mode & S_IROTH) === S_IROTH) permissions += "r";
  else permissions += "-";
  if ((mode & S_IWOTH) === S_IWOTH) permissions += "w";
  else permissions += "-";
  if ((mode & S_IXOTH) === S_IXOTH) permissions += "x";
  else permissions += "-";

  return permissions;
}

export function convertBytesToHumanReadable(bytes: number): string {
  const unitLabels = ["Byte", "KB", "MB", "GB", "TB"];
  let unitIndex = 0;

  while (bytes >= 1024 && unitIndex < unitLabels.length - 1) {
    bytes /= 1024;
    unitIndex++;
  }

  return `${bytes.toFixed(2)} ${unitLabels[unitIndex]}`;
}
