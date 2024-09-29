import Mock from 'mockjs';
import { GetFileDetailRequest, GetFileDetailResponse } from '../GetFileDetail';
import { pickFileType } from './utils';
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
