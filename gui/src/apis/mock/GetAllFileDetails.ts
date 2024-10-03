import { GetAllFileDetailsRequest } from '../GetAllFileDetails';
import { cachedFileList, findFilesByPath, generateRandomFileDetail, randomNumber } from './utils';

export function mock(request: GetAllFileDetailsRequest) {
  const { path } = request;
  const files = findFilesByPath(cachedFileList, path, '/');
  if (!files) {
    return { files: [] };
  }
  return { files: files};
}
