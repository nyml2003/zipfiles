import { Filter } from '@/types';
import { createSlice, PayloadAction } from '@reduxjs/toolkit';

export interface FileState {
  currentFile: string;
  currentPath: string;
  selectedFile: string[];
  filter: Partial<Filter>;
  view: 'table' | 'tree';
  isFiltering: boolean;
  backupConfig: {
    compress: boolean;
    encrypt: boolean;
    password?: string;
  };
}

const initialState: FileState = {
  currentFile: '',
  currentPath: '',
  selectedFile: [],
  filter: {},
  view: 'tree',
  isFiltering: false,
  backupConfig: {
    compress: true,
    encrypt: false,
  },
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
    updateCurrentView(state, action: PayloadAction<'table' | 'tree'>) {
      state.view = action.payload;
    },
    updateIsFiltering(state, action: PayloadAction<boolean>) {
      state.isFiltering = action.payload;
    },
    updateBackupConfig(state, action: PayloadAction<FileState['backupConfig']>) {
      state.backupConfig = action.payload;
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
  updateCurrentView,
  updateIsFiltering,
  updateBackupConfig,
} = fileSlice.actions;

export const fileReducer = fileSlice.reducer;

export default fileReducer;
