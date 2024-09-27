import Mock from 'mockjs';
export interface GetFileDetailRequest {
  path: string;
}

enum FileType {
  None = 0,
  NotFound = -1,
  Regular = 1,
  Directory = 2,
  Symlink = 3,
  Block = 4,
  Character = 5,
  Fifo = 6,
  Socket = 7,
  Unknown = 8,
}

export function FileTypeToString(type: FileType): string {
  switch (type) {
    case FileType.None:
      return 'None';
    case FileType.NotFound:
      return 'NotFound';
    case FileType.Regular:
      return 'Regular';
    case FileType.Directory:
      return 'Directory';
    case FileType.Symlink:
      return 'Symlink';
    case FileType.Block:
      return 'Block';
    case FileType.Character:
      return 'Character';
    case FileType.Fifo:
      return 'Fifo';
    case FileType.Socket:
      return 'Socket';
    case FileType.Unknown:
      return 'Unknown';
  }
}

export interface FileDetail {
  name: string;
  type: FileType;
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
