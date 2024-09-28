import { FileType } from '@/types';
import { pickFileType } from '@/utils';
import Mock from 'mockjs';
export interface GetFileDetailRequest {
  path: string;
}

export interface FileDetail {
  name: string;
  type: FileType;
  createTime: string;
  updateTime: string;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}

export interface GetFileDetailResponse extends FileDetail {}

export function mock(request: GetFileDetailRequest): GetFileDetailResponse {
  return {
    ...Mock.mock({
      name: request.path.split('/').pop(),
      createTime: '@datetime',
      updateTime: '@datetime',
      size: '@integer(1024, 10240000)',
      owner: '@word',
      group: '@word',
      mode: '@integer(0, 511)',
      path: request.path,
    }),
    type: pickFileType(),
  };
}
