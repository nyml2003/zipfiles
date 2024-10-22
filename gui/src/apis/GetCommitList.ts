
interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  storagePath: string;
  author: string;
  isEncrypt: boolean;
  isDelete: boolean;
}

export interface GetCommitListRequest {}

export interface GetCommitListResponse {
  commits: CommitLog[];
}
