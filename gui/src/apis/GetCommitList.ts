
export interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  lca: string; // lca is the lowest common ancestor of all files
  storagePath: string;
  author: string;
  isEncrypt: boolean;
  isDelete: boolean;
}

export interface GetCommitListRequest {}

export interface GetCommitListResponse {
  data: CommitLog[];
}
