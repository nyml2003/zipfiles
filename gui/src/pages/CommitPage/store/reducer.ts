import { buildTree } from '@/utils';
import { FileType } from '@/types';
interface FileDetail {
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
export const updatePath = (state: State, action: PayloadAction<string>) => {
  return { ...state, path: action.payload };
};

export const updateFile = (state: State, action: PayloadAction<string>) => {
  return { ...state, file: action.payload };
};

export const updateCommitId = (state: State, action: PayloadAction<string>) => {
  return { ...state, commitId: action.payload };
};

export const updateFiles = (state: State, action: PayloadAction<FileDetail[]>) => {
  const newFiles = buildTree(action.payload);
  const newPath = newFiles.path;
  return { ...state, files: newFiles, path: newPath };
};
