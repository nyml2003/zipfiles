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

export interface State {
  path: string;
  file: string;
  commitId: string;
  files: Dir<FileDetail>;
  loading: boolean;
}
