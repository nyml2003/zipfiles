import { omit } from 'lodash';
import { GetFileDetailListRequest, GetFileDetailListResponse } from '../GetFileDetailList';
import { findFilesByPath } from './utils';
import { MockFileDetail } from './types';
import { cachedFileRoot } from './init';

export function mock(request: GetFileDetailListRequest): GetFileDetailListResponse {
  const { path } = request;
  const filter = request.filter || {};
  const files: MockFileDetail[] | null = findFilesByPath(cachedFileRoot, path, filter);
  if (!files) {
    throw new Error('File not found' + path);
  }
  return {
    files: files.map(file => omit(file, 'children')),
  };
}
