/* eslint-disable prefer-const */
import { FileType } from '@/types';
import Mock from 'mockjs';
import { filterBy } from '@/utils';
import { v4 as uuidv4 } from 'uuid';
const MockFileNumber: [number, number] = [100, 200];

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
export const cachedFileList: MockFileDetail[] = [
  {
    name: '/',
    type: FileType.Directory,
    children: Array.from({ length: randomNumber(...MockFileNumber) }).map(() =>
      generateRandomFileDetail('/'),
    ),
    createTime: new Date().getTime() / 1000,
    updateTime: new Date().getTime() / 1000,
    size: 0,
    owner: 'root',
    group: 'root',
    mode: 777,
    path: '/',
  },
];

// 随机从文件列表cachedFileList中选择若干个文件
function pickFiles(): MockFileDetail[] {
  const files: MockFileDetail[] = [];
  const fileNumber = randomNumber(1, 10);
  const flatFiles = cachedFileList.reduce((acc: MockFileDetail[], file: MockFileDetail) => {
    acc.push(file);
    if (file.children) {
      acc.push(...file.children);
    }
    return acc;
  }, []);
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
  backups.set(uuidv4(), files);
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

export function generateRandomFileDetail(path = '/'): MockFileDetail {
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
    path: path === '/' ? `/${name}` : `${path}/${name}`,
  };
}
function calPath(currentPath: string, file: MockFileDetail) {
  if (currentPath === '/' && file.name === '/') return '/';
  if (currentPath === '/') {
    return `/${file.name}`;
  }
  return `${currentPath}/${file.name}`;
}
// 递归查找函数
export function findFilesByPath(
  files: MockFileDetail[],
  targetPath: string,
  currentPath: string,
  filter?: Partial<Filter>,
): MockFileDetail[] | null {
  for (const file of files) {
    const newCurrentPath = calPath(currentPath, file);
    if (newCurrentPath === targetPath) {
      if (file.type !== FileType.Directory) {
        return [file];
      }
      if (!file.children) {
        file.children = Array.from({ length: randomNumber(...MockFileNumber) }, () =>
          generateRandomFileDetail(newCurrentPath),
        );
      }
      const result = file.children;
      if (filter && Object.keys(filter).length) {
        return filterBy(result, filter);
      }
      return result;
    }
    if (file.type === FileType.Directory) {
      const result = findFilesByPath(file.children || [], targetPath, newCurrentPath, filter);
      if (result) {
        return result;
      }
    }
  }
  return null;
}

export function findFile(
  files: MockFileDetail[],
  targetPath: string,
  currentPath: string,
): MockFileDetail | null {
  for (const file of files) {
    const newCurrentPath = calPath(currentPath, file);
    if (newCurrentPath === targetPath) {
      return file;
    }
    if (file.type === FileType.Directory) {
      const result = findFile(file.children || [], targetPath, newCurrentPath);
      if (result) {
        return result;
      }
    }
  }
  return null;
}

export function randomNumber(min: number, max: number) {
  return Math.floor(Math.random() * (max - min + 1) + min);
}
