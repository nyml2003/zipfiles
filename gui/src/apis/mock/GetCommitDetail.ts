import { GetCommitDetailRequest, GetCommitDetailResponse } from '../GetCommitDetail';
import { backups, cachedCommitList } from './utils';

export function mock(request: GetCommitDetailRequest): GetCommitDetailResponse {
  const files = backups.get(request.uuid);
  if (!files) {
    throw new Error('no such commit');
  }
  return {
    files,
  };
}
