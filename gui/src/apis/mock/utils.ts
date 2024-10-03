import { FileType } from '@/types';
import Mock from 'mockjs';
import { File } from '../GetFileList';
import { FileDetail } from '../GetFileDetail';

export function pickIndex(choices: any[], _weights?: number[]): number {
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
  return pickIndex(fileTypes, weights);
}
export let cachedFileList: MockFileDetail[] = [
  {
    name: '/',
    type: FileType.Directory,
    children: Array.from({ length: randomNumber(15, 20) }).map(() => generateRandomFileDetail('/')),
    createTime: new Date().getTime() / 1000,
    updateTime: new Date().getTime() / 1000,
    size: 0,
    owner: 'root',
    group: 'root',
    mode: 777,
    path: '/',
  },
];
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
    name: name,
    type,
    children: null,
    //随机生成数值时间戳
    createTime: new Date(Mock.mock('@datetime')).getTime() / 1000,
    updateTime: new Date(Mock.mock('@datetime')).getTime() / 1000,
    size: type === FileType.Regular ? Mock.mock('@integer(1024, 1024 * 1024)') : 0,
    owner: Mock.mock('@name'),
    group: Mock.mock('@name'),
    mode: Mock.mock('@integer(0, 777)'),
    path: path === '/' ? `/${name}` : `${path}/${name}`,
  };
}
// 递归查找函数
export function findFilesByPath(files: MockFileDetail[], targetPath: string, currentPath: string): MockFileDetail[] | null {
  for (const file of files) {
    function calPath() {
      if (currentPath === '/' && file.name === '/') return '/';
      if (currentPath === '/') {
        return `/${file.name}`;
      } else {
        return `${currentPath}/${file.name}`;
      }
    }
    const newCurrentPath = calPath();
    if (newCurrentPath === targetPath) {
      if (!file.children) {
        file.children = Array.from({ length: randomNumber(15, 30) }, () => generateRandomFileDetail(newCurrentPath));
      }
      return file.children;
    }
    if (file.type === FileType.Directory) {
      const result = findFilesByPath(file.children || [], targetPath, newCurrentPath);
      if (result) {
        return result;
      }
    }
  }
  return null;
}

export function findFile(files: MockFileDetail[], targetPath: string, currentPath: string): MockFileDetail | null {
  for (const file of files) {
    function calPath() {
      if (currentPath === '/' && file.name === '/') return '/';
      if (currentPath === '/') {
        return `/${file.name}`;
      } else {
        return `${currentPath}/${file.name}`;
      }
    }
    const newCurrentPath = calPath();
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