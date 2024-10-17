import { Filter } from '@/types';
import { createSlice, PayloadAction } from '@reduxjs/toolkit';

export interface CreateCommitState {
  currentFile: string;
  currentPath: string;
  selectedFile: string[];
  expandedSelectedFile: string[];
  filter: Partial<Filter>;
  isFiltering: boolean;
}

const initialState: CreateCommitState = {
  currentFile: '',
  currentPath: '',
  selectedFile: [],
  expandedSelectedFile: [],
  filter: {},
  isFiltering: false,
};

const CreateCommitReducer = createSlice({
  name: 'createCommit',
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
    updateIsFiltering(state, action: PayloadAction<boolean>) {
      state.isFiltering = action.payload;
    },
    resetSelectedFile(state) {
      state.selectedFile = [];
    },
    updateExpandedSelectedFile(state, action: PayloadAction<string[]>) {
      state.expandedSelectedFile = action.payload;
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
  updateIsFiltering,
  resetSelectedFile,
  updateExpandedSelectedFile,
} = CreateCommitReducer.actions;

export default CreateCommitReducer.reducer;
