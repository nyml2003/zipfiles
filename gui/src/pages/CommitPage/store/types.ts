import { FileType } from '@/types';

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

type Dir = { path: string; children: (Dir | FileDetail)[]; name: string };

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export declare type PayloadAction<P = any> = {
  payload: P;
};

export interface State {
  path: string;
  file: string;
  commitId: string;
  files: Dir;
}
