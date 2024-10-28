import { omit } from 'lodash';
import { GetFileDetailRequest, GetFileDetailResponse } from '../GetFileDetail';
import { findFile } from './utils';

// 全局变量来缓存嵌套的目录结构

export function mock(request: GetFileDetailRequest): GetFileDetailResponse {
  const { path, name } = request;
  const file = omit(findFile(path, name), 'children');
  if (!file) {
    throw new Error('no such file' + path);
  }
  return file;
}
