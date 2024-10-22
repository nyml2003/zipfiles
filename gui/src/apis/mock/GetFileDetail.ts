import { GetFileDetailRequest, GetFileDetailResponse } from '../GetFileDetail';
import { cachedFileList, findFile } from './utils';

// 全局变量来缓存嵌套的目录结构

export function mock(request: GetFileDetailRequest): GetFileDetailResponse {
  const { path } = request;
  const file = findFile(cachedFileList, path, '/');
  if (!file) {
    throw new Error('no such file');
  }
  return file;
}
