import Mock from 'mockjs';
export interface GetFileListRequest {
  path: string;
}
export interface File {
  name: string;
  type: string;
}
export interface GetFileListResponse {
  files: File[];
}

export function mock(request: GetFileListRequest): GetFileListResponse {
  return Mock.mock({
    'files|5-10': [
      {
        name: '@word',
        type: '@pick(["file", "directory"])',
      },
    ],
  });
}
