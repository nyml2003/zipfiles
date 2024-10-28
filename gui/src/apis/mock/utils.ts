/* eslint-disable prefer-const */
import { FileType } from '@/types';
import { cleanObject } from '@/utils';
import Mock from 'mockjs';
import { v4 as uuidv4 } from 'uuid';
import { GetFileDetailListRequest } from '../GetFileDetailList';
import { omit } from 'lodash';
const MockFileNumber: [number, number] = [10, 20];

interface File {
  name: string;
  type: FileType;
}

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

interface RootFileDetail {
  name: string;
  type: FileType.Directory;
  children: MockFileDetail[];
}

interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  storagePath: string;
  author: string;
  isEncrypt: boolean;
  isDelete: boolean;
}

type Filter = Partial<{
  type: FileType;
  name: string;
  minSize: number;
  maxSize: number;
  minCreateTime: number;
  maxCreateTime: number;
  minUpdateTime: number;
  maxUpdateTime: number;
  owner: string;
  group: string;
}>;

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
export const filterBy = <T extends FileDetail>(
  files: T[],
  filter: Partial<GetFileDetailListRequest['filter']>,
): T[] => {
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
    if (cleanedFilter.group && !file.group.includes(cleanedFilter.group)) return false;
    return true;
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
    FileType.Unknown,
  ];
  const weights = [1, 1, 3, 5, 1, 1, 1, 1, 1, 1];
  return pickIndex<FileType>(fileTypes, weights);
}
export const cachedFileRoot: RootFileDetail = {
  name: '',
  type: FileType.Directory,
  children: Array.from({ length: randomNumber(...MockFileNumber) }).map(() =>
    generateRandomFileDetail(''),
  ),
};

cachedFileRoot.children.push({
  name: 'usr',
  type: FileType.Directory,
  children: Array.from({ length: randomNumber(...MockFileNumber) }).map(() =>
    generateRandomFileDetail('usr'),
  ),
  createTime: new Date(Mock.mock('@datetime')).getTime() / 1000,
  updateTime: new Date(Mock.mock('@datetime')).getTime() / 1000,
  size: 0,
  owner: Mock.mock('@name'),
  group: Mock.mock('@name'),
  mode: Mock.mock('@integer(0, 777)'),
  path: '',
});

function generateWholeFileTree(root: MockFileDetail[], depth: number) {
  if (depth === 0) return;
  root.forEach(file => {
    if (file.type === FileType.Directory) {
      file.children = Array.from({ length: randomNumber(...MockFileNumber) }).map(() =>
        generateRandomFileDetail(file.path + '/' + file.name),
      );
      if (depth === 1) {
        file.children = file.children.filter(file => file.type !== FileType.Directory);
      }
      generateWholeFileTree(file.children, depth - 1);
    }
  });
}

generateWholeFileTree(cachedFileRoot.children, 3);

// 随机从文件列表cachedFileList中选择若干个文件
function pickFiles(): MockFileDetail[] {
  const files: MockFileDetail[] = [];
  const fileNumber = randomNumber(1, 10);
  const flatFiles = cachedFileRoot.children.reduce(
    (acc: MockFileDetail[], file: MockFileDetail) => {
      acc.push(file);
      if (file.children) {
        acc.push(...file.children);
      }
      return acc;
    },
    [],
  );
  for (let i = 0; i < fileNumber; i++) {
    const index = Math.floor(Math.random() * flatFiles.length);
    files.push(flatFiles[index]);
  }
  return files;
}
export let backups: Map<string, FileDetail[]> = new Map();

export let cachedCommitList: CommitLog[] = [];

function generateCommitLog() {
  const files = pickFiles();
  const uuid = uuidv4();
  backups.set(
    uuid,
    files.map(file => omit(file, 'children')),
  );
  cachedCommitList.push({
    uuid,
    message: Mock.mock('@sentence'),
    createTime: new Date(Mock.mock('@datetime')).getTime() / 1000,
    storagePath: '/usr/local/zipfiles',
    author: Mock.mock('@name'),
    isEncrypt: Mock.mock('@boolean'),
    isDelete: false,
  });
}

Array.from({ length: randomNumber(5, 10) }).forEach(() => generateCommitLog());

export interface MockFile extends File {
  children: MockFile[] | null;
}

export interface MockFileDetail extends FileDetail {
  children: MockFileDetail[] | null;
}

export function generateRandomFileDetail(path: string): MockFileDetail {
  const type = pickFileType(); // 随机选择文件类型
  const name = Mock.mock('@word(3, 10)');
  return {
    name,
    type,
    children: null,
    // 随机生成数值时间戳
    createTime: new Date(Mock.mock('@datetime')).getTime() / 1000,
    updateTime: new Date(Mock.mock('@datetime')).getTime() / 1000,
    size: type === FileType.Regular ? Mock.mock('@integer(1024, 1024 * 1024)') : 0,
    owner: Mock.mock('@name'),
    group: Mock.mock('@name'),
    mode: Mock.mock('@integer(0, 777)'),
    path,
  };
}
// function calPath(currentPath: string, file: MockFileDetail) {
//   if (currentPath === '/' && file.name === '/') return '/';
//   if (currentPath === '/') {
//     return `/${file.name}`;
//   }
//   return `${currentPath}/${file.name}`;
// }
// 递归查找函数
export function findFilesByPath(
  targetPath: string,
  filter?: Partial<Filter>,
): MockFileDetail[] | null {
  if (targetPath === '') {
    return filterBy(cachedFileRoot.children, filter);
  }
  const parts = targetPath.split('/').slice(1);
  let current: MockFileDetail = cachedFileRoot.children.find(
    file => file.name === parts[0],
  ) as MockFileDetail;
  for (let i = 1; i < parts.length; i++) {
    if (!current.children) {
      throw new Error('File not found' + targetPath);
    }
    current = current.children.find(file => file.name === parts[i]) as MockFileDetail;
  }
  // if (!current.children && current.type === FileType.Directory) {
  //   current.children = Array.from({ length: randomNumber(...MockFileNumber) }, () =>
  //     generateRandomFileDetail(current.path + '/' + current.name),
  //   );
  // }
  return filterBy(current.children || [], filter);
}

export function findFile(targetPath: string, targetName: string): MockFileDetail | null {
  const files = findFilesByPath(targetPath);
  if (!files) return null;
  return files.find(file => file.name === targetName) || null;
}

export function randomNumber(min: number, max: number) {
  return Math.floor(Math.random() * (max - min + 1) + min);
}
