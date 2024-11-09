import { buildTree } from '@/utils';
import { State } from './types';
import { FileDetail } from './types';

export const updatePath = (state: State, payload: string) => {
  return { ...state, path: payload };
};

export const updateFile = (state: State, payload: string) => {
  return { ...state, file: payload };
};

export const updateCommitId = (state: State, payload: string) => {
  return { ...state, commitId: payload };
};

export const updateFiles = (state: State, payload: FileDetail[]) => {
  const newFiles = buildTree(payload);
  // console.log(JSON.stringify(newFiles, null, 2));
  // console.log(JSON.stringify(payload.payload, null, 2));
  return { ...state, files: newFiles, path: '' };
};

export const updateLoading = (state: State, payload: boolean) => {
  return { ...state, loading: payload };
};
