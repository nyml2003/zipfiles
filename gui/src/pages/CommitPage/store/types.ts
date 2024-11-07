import { FileType } from '@/types';
import { Dir } from '@/utils';

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

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export declare type PayloadAction<P = any> = {
  payload: P;
};

export interface State {
  path: string;
  file: string;
  commitId: string;
  files: Dir<FileDetail>;
}
