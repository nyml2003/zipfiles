import Mock from 'mockjs';

export interface GetFileListRequest {
  path: string;
}

export interface GetFileListResponse {
  files: File[];
}

export interface File {
  name: string;
  type: string;
}

interface MockFile extends File {
  children: MockFile[] | null;
}

// 全局变量来缓存嵌套的目录结构
let cachedFileList: MockFile[] = [
  {
    name: 'app',
    type: 'directory',
    children: [
      {
        name: 'src',
        type: 'directory',
        children: [
          {
            name: 'apis',
            type: 'directory',
            children: [
              {
                name: 'GetFileList.ts',
                type: 'file',
                children: null,
              },
            ],
          },
        ],
      },
    ],
  },
];

function generateRandomFiles(): MockFile {
  return Mock.mock({
    name: '@word',
    type: '@pick(["file", "directory"])',
    children() {
      if (this.type === 'directory') {
        return generateRandomFiles();
      }
      return null;
    },
  });
}

// 递归查找函数
function findFilesByPath(files: MockFile[], path: string[]): MockFile[] | null {
  console.log(files, path);
  for (const file of files) {
    const [currentPath, ...restPath] = path;
    if (file.name === currentPath) {
      return file.children;
    }
      if (file.children) {
        console.log(file.children, restPath);
      const result = findFilesByPath(file.children, restPath);
      if (result) {
        return result;
      }
    }
  }
  return null;
}

export function mock(request: GetFileListRequest): GetFileListResponse {
  const { path } = request;
  const files = findFilesByPath(cachedFileList, path.split('/'));
  return { files: files || [] };
}
