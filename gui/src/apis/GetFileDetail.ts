import { FileType } from '@/types';
export interface GetFileDetailRequest {
  path: string;
}

export interface FileDetail {
  name: string;
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}

export interface GetFileDetailResponse extends FileDetail {}
