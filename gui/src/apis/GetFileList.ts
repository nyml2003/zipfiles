import { FileType } from '@/types';

export interface GetFileListRequest {
  path: string;
}

export interface GetFileListResponse {
  files: File[];
}

export interface File {
  name: string;
  type: FileType;
}