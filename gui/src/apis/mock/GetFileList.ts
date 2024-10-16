
import { GetFileListRequest, GetFileListResponse, Filter } from '../GetFileList';
import { cachedFileList, findFilesByPath } from './utils';

// 全局变量来缓存嵌套的目录结构

export function mock(request: GetFileListRequest): GetFileListResponse {
  const { path } = request;
  const filter : Partial<Filter> = request.filter || {};
  const files = findFilesByPath(cachedFileList, path, '/', filter);
  if (!files) {
    return { files: [] };
  }
  return { files: files.map(file => ({ name: file.name, type: file.type })) };
}
