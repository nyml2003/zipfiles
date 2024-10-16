import { CommitLog } from './GetCommitList';

export interface PostBackupRequest {
  files: string[]; // path
  key: string;
  lca: string; // lca is the lowest common ancestor of all files
  cl: CommitLog;
}

export interface PostBackupResponse {}

export function mock(request: PostBackupRequest): PostBackupResponse {
  return {};
}
