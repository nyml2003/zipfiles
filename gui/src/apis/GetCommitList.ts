
export interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  lca: string;
  storagePath: string;
  author: string;
  isEncrypt: boolean;
  isDelete: boolean;
}

export interface GetCommitListRequest {}

export interface GetCommitListResponse {
  data: CommitLog[];
}
