import { GetFileDetailRequest, GetFileDetailResponse } from '../GetFileDetail';
import { cachedFileList, findFile } from './utils';

export function mock(request: GetFileDetailRequest): GetFileDetailResponse {
  const { path } = request;
  const file = findFile(cachedFileList, path, '/');
  if (!file) {
    throw new Error('File not found'+path);
  }
  return file;
}
