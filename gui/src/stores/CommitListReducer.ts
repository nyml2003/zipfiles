import { CommitLog } from '@/apis/GetCommitList';
import { NestedFileDetail } from '@/types';
import { createSlice, PayloadAction } from '@reduxjs/toolkit';

export interface CommitListState {
  currentFile: string; // 当前选中的活跃文件
  currentPath: string; // 当前的根路径
  currentCommit: CommitLog | null; // 当前选中的提交记录
  files: NestedFileDetail[] | null; // 当前路径下的文件列表
}

const initialState: CommitListState = {
  currentFile: '',
  currentPath: '',
  currentCommit: null,
  files: null,
};

const commitListSlice = createSlice({
  name: 'commitList',
  initialState,
  reducers: {
    setCurrentFile: (state, action: PayloadAction<string>) => {
      state.currentFile = action.payload;
    },
    setCurrentCommit: (state, action: PayloadAction<CommitLog>) => {
      state.currentCommit = action.payload;
    },
    setFiles: (state, action: PayloadAction<NestedFileDetail[]>) => {
      state.files = action.payload;
    },
    setCurrentPath: (state, action: PayloadAction<string>) => {
      state.currentPath = action.payload;
    },
  },
});

export const { setCurrentFile, setCurrentCommit, setFiles, setCurrentPath } = commitListSlice.actions;

export default commitListSlice.reducer;
