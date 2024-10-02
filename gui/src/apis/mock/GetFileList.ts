
import { FileType } from '@/types';
import { GetFileListRequest, GetFileListResponse, File } from '../GetFileList';
import { pickFileType } from './utils';
import Mock from 'mockjs';

interface MockFile extends File {
  children: MockFile[] | null;
}

// 全局变量来缓存嵌套的目录结构
let cachedFileList: MockFile[] = [
  {
    name: '/',
    type: FileType.Directory,
    children: Array.from({ length: 5 }, () => generateRandomFiles()),
  },
];

function generateRandomFiles(): MockFile {
  const type = pickFileType(); // 随机选择文件类型
  return {
    name: Mock.mock('@word(3, 10)'),
    type,
    children: null,
  };
}

// 递归查找函数
function findFilesByPath(
  files: MockFile[],
  targetPath: string,
  currentPath: string,
): MockFile[] | null {
  for (const file of files) {
    const newPath = currentPath ? `${currentPath}/${file.name}` : file.name;
    if (newPath === targetPath) {
      if (!file.children) {
        file.children = Array.from({ length: 5 }, () => generateRandomFiles());
      }
      return file.children;
    }
    if (file.type === FileType.Directory) {
      const result = findFilesByPath(file.children || [], targetPath, newPath);
      if (result) {
        return result;
      }
    }
  }
  return null;
}

export function mock(request: GetFileListRequest): GetFileListResponse {
  const { path } = request;
  const files = findFilesByPath(cachedFileList, `/${path}`, '');
  if (!files) {
    return { files: [] };
  }
  return { files: files.map(file => ({ name: file.name, type: file.type })) };
}
