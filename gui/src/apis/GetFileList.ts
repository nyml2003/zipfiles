import Mock from 'mockjs';

export interface GetFileListRequest {
  path: string;
}

export interface GetFileListResponse {
  files: File[];
}

export interface File {
  name: string;
  type: "none" | "not_found" | "regular" | "directory" | "symlink" | "block" | "character" | "fifo" | "socket" | "unknown";
}

interface MockFile extends File {
  children: MockFile[] | null;
}

// 全局变量来缓存嵌套的目录结构
let cachedFileList: MockFile[] = [
  {
    name: '/',
    type: 'directory',
    children: Array.from({ length: 15 }, () => generateRandomFiles()),
  },
];

function generateRandomFiles(): MockFile {
  return Mock.mock({
    name: '@word',
    type: '@pick(["directory", "regular", "symlink", "block", "character", "fifo", "socket", "unknown"])',
  });
}

// 递归查找函数
function findFilesByPath(
  files: MockFile[],
  targetPath: string,
  currentPath: string,
): MockFile[] | null {
  //console.log('findFilesByPath', files, targetPath, currentPath);
  for (const file of files) {
    const newPath = currentPath ? `${currentPath}/${file.name}` : file.name;
    if (newPath === targetPath) {
      if (!file.children) {
        file.children = Array.from({ length: 15 }, () => generateRandomFiles());
      }
      return file.children;
    }
    if (file.type === 'directory') {
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
