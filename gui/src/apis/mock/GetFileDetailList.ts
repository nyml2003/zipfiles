import { GetFileDetailListRequest, GetFileDetailListResponse } from '../GetFileDetailList';
import { cachedFileList, findFilesByPath } from './utils';

export function mock(request: GetFileDetailListRequest): GetFileDetailListResponse {
  const { path } = request;
  const filter = request.filter || {};
  const files = findFilesByPath(cachedFileList, path, '/', filter);
  if (!files) {
    throw new Error('File not found' + path);
  }
  return { files };
}
