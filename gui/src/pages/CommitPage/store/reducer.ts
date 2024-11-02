import { buildTree } from '@/utils';
import { PayloadAction, State } from './types';
import { FileDetail } from './types';

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
