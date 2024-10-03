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

export interface Filter {
  type: FileType;
  size: {
    min: number;
    max: number;
  };
  owner: string;
  group: string;
}
                                        