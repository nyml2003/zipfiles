import { FileType } from '@/types';
export interface GetAllFileDetailsRequest {
  path: string;
}

export interface FileDetail {
  name: string;
  type: FileType;
  createTime: string;
  updateTime: string;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}

export interface GetAllFileDetailsResponse {
  files : FileDetail[];
}
