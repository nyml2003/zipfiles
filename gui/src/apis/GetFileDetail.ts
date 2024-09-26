import Mock from 'mockjs';
export interface GetFileDetailRequest {
  path: string;
}

export interface FileDetail {
  name: string;
  type:
    | 'none'
    | 'not_found'
    | 'regular'
    | 'directory'
    | 'symlink'
    | 'block'
    | 'character'
    | 'fifo'
    | 'socket'
    | 'unknown';
  createTime: Date;
  updateTime: Date;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}

export interface GetFileDetailResponse {
  metadata: FileDetail;
}

export function mock(request: GetFileDetailRequest): GetFileDetailResponse {
  return {
    metadata: Mock.mock({
      name: request.path.split('/').pop(),
      type: '@pick(["directory", "regular", "symlink", "block", "character", "fifo", "socket", "unknown"])',
      createTime: '@datetime',
      updateTime: '@datetime',
      size: '@integer(1024, 10240000)',
      owner: '@word',
      group: '@word',
      mode: '@integer(0, 511)',
      path: request.path,
    }),
  };
}
