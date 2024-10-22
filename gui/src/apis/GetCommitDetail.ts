import { FileType } from '@/types';

interface FileDetail {
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
  name: string;

}

export interface GetCommitDetailRequest {
  uuid: string;
}

export interface GetCommitDetailResponse {
    files: FileDetail[];
}
