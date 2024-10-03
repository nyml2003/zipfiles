import { Filter } from '@/types';
import { createSlice, PayloadAction } from '@reduxjs/toolkit';

export interface FileState {
  currentFile: string;
  currentPath: string;
  selectedFile: string[];
  filter: Partial<Filter>;
}

const initialState: FileState = {
  currentFile: '',
  currentPath: '',
  selectedFile: [],
  filter: {},
};

const fileSlice = createSlice({
  name: 'file',
  initialState,
  reducers: {
    updateCurrentFile(state, action: PayloadAction<string>) {
      state.currentFile = action.payload;
    },
    updateSelectedFile(state, action: PayloadAction<string[]>) {
      state.selectedFile = action.payload;
    },
    updateCurrentPath(state, action: PayloadAction<string>) {
      state.currentPath = action.payload;
    },
    handleRefresh(state) {
      state.currentFile = '';
      state.currentPath = '';
      state.selectedFile = [];
      state.filter = {};
    },
    updateFilter(state, action: PayloadAction<Partial<Filter>>) {
      state.filter = action.payload;
    },
    resetFilter(state) {
      state.filter = initialState.filter;
    },
  },
});

export const {
  updateCurrentFile,
  updateSelectedFile,
  updateCurrentPath,
  handleRefresh,
  updateFilter,
  resetFilter,
} = fileSlice.actions;

export const fileReducer = fileSlice.reducer;

export default fileReducer;
