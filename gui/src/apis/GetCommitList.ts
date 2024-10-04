
export interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  defaultPath: string;
  storagePath: string;
  isEncrypt: boolean;
  isDelete: boolean;
  author: string;
}

export interface GetCommitListRequest {}

export interface GetCommitListResponse {
  data: CommitLog[];
}
