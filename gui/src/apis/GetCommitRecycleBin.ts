interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  storagePath: string;
  author: string;
  isEncrypt: boolean;
  isDelete: boolean;
}

export interface GetCommitRecycleBinRequest {}

export interface GetCommitRecycleBinResponse {
  commits: CommitLog[];
}
