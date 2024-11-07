import { FileType } from '@/types';
import { omit } from 'lodash';
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
export function selectEnvironment<T>(production: T, development: T) {
  if (process.env.BASE_ENV === 'prod') {
    return production;
  }
  return development;
}

export function isProduction() {
  return process.env.BASE_ENV === 'prod';
}

export function isDevelopment() {
  return process.env.BASE_ENV === 'dev';
}

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export function cleanObject(obj: any) {
  if (obj === null || obj === undefined) return obj;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  return Object.entries(obj).reduce((acc: any, [key, value]) => {
    if (value && typeof value === 'object') {
      value = cleanObject(value);
    }
    if (value !== null && value !== undefined) {
      acc[key] = value;
    }
    return acc;
  }, {});
}

// export const filterBy = <T extends FileDetail>(
//   files: T[],
//   filter: Partial<GetFileListRequest['filter']>,
// ): T[] => {
//   const cleanedFilter = cleanObject(filter);
//   if (!cleanedFilter || Object.keys(cleanedFilter).length === 0) return files;

//   return files.filter(file => {
//     if (file.type === FileType.Directory) return true;
//     if (cleanedFilter.type && file.type !== cleanedFilter.type) return false;
//     if (cleanedFilter.size) {
//       if (cleanedFilter.size.min && file.size < cleanedFilter.size.min) return false;
//       if (cleanedFilter.size.max && file.size > cleanedFilter.size.max) return false;
//     }
//     if (cleanedFilter.owner && !file.owner.includes(cleanedFilter.owner)) return false;
//     if (cleanedFilter.group && !file.group.includes(cleanedFilter.group)) return false;
//     return true;
//   });
// };

export function findLongestCommonPrefix(paths: string[]) {
  if (paths.length === 0) return '';
  if (paths.length === 1) return paths[0];

  // 将路径分割成数组
  const splitPaths = paths.map(path => path.split('/'));

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
  const lca = commonPrefix.join('/');
  // 将数组转换回字符串，并用斜杠连接
  return lca;
}
export type Dir<DataType> = {
  children: DataType[];
  subDir: Dir<DataType>[];
  name: string;
};
export function buildTree<DataType extends { path: string }>(files: DataType[]): Dir<DataType> {
  const root: Dir<DataType> = { children: [], name: '', subDir: [] };
  files.forEach(file => {
    if (file.path === '.') {
      file.path = '';
      root.children.push(file);
      return;
    }
    const parts = file.path.split('/');
    let current = root;
    for (let i = 0; i < parts.length; i++) {
      const part = parts[i];
      const childIndex = current.subDir.findIndex(subDir => subDir.name === part);
      if (childIndex === -1) {
        current.subDir.push({
          name: part,
          subDir: [],
          children: [],
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

export function findFile<DataType extends {
  name: string; type: FileType;
}>(
  files: Dir<DataType>,
  targetPath: string,
): ({ name: string; type: FileType })[] {
  let current = files;
  if (targetPath !== '') {
    const parts = targetPath.split('/');
    for (let i = 0; i < parts.length; i++) {
      const part = parts[i];
      const child = current.subDir.find(subDir => subDir.name === part);
      if (!child) return [];
      current = child;
    }
  }
  const result: { name: string; type: FileType }[] = current.children;
  current.subDir.forEach(subDir => {
    if (! result.find(file => file.name === subDir.name)) {
      result.push({
        name: subDir.name,
        type: FileType.Directory,
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
