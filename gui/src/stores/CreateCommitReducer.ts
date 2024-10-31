import { FileType } from '@/types';
import { createSlice, PayloadAction } from '@reduxjs/toolkit';
type Filter = Partial<{
  type: FileType;
  name: string;
  minSize: number;
  maxSize: number;
  minCreateTime: number;
  maxCreateTime: number;
  minUpdateTime: number;
  maxUpdateTime: number;
  owner: string;
  group: string;
}>;

type File = {
  name: string;
  path: string;
};

export interface CreateCommitState {
  currentFile: string;
  currentPath: string;
  selectedFile: {
    files: File[];
    directories: string[];
  };
  backupFiles: string[];
  filter: Partial<Filter>;
  isFiltering: boolean;
}

const initialState: CreateCommitState = {
  currentFile: '',
  currentPath: '',
  selectedFile: {
    files: [],
    directories: [],
  },
  filter: {},
  isFiltering: false,
  backupFiles: [],
};

const CreateCommitReducer = createSlice({
  name: 'createCommit',
  initialState,
  reducers: {
    updateCurrentFile(state, action: PayloadAction<string>) {
      state.currentFile = action.payload;
    },
    updateCurrentPath(state, action: PayloadAction<string>) {
      state.currentPath = action.payload;
    },
    handleRefresh(state) {
      state.currentFile = '';
      state.currentPath = '';
      state.selectedFile = {
        files: [],
        directories: [],
      };
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
    updateSelectedFile(state, action: PayloadAction<File[]>) {
      state.selectedFile.files = action.payload;
    },
    addSelectedFile(state, action: PayloadAction<File>) {
      if (
        !state.selectedFile.files.find(
          file => file.path === action.payload.path && file.name === action.payload.name,
        )
      ) {
        state.selectedFile.files.push(action.payload);
      }
    },
    checkSelectedFile(state, action: PayloadAction<File>) {
      const index = state.selectedFile.files.findIndex(
        file => file.path === action.payload.path && file.name === action.payload.name,
      );
      if (index !== -1) {
        state.selectedFile.files.splice(index, 1);
      } else {
        state.selectedFile.files.push(action.payload);
      }
    },
    checkSelectedDirectory(state, action: PayloadAction<string>) {
      if (action.payload === '') {
        if (state.selectedFile.directories.includes('')) {
          state.selectedFile.directories = [];
        } else {
          state.selectedFile.directories = [''];
        }
        state.selectedFile.files = [];
        return;
      }
      if (state.selectedFile.directories.includes(action.payload)) {
        state.selectedFile.directories = state.selectedFile.directories.filter(
          directory => directory !== action.payload,
        );
      } else {
        state.selectedFile.directories.push(action.payload);
        state.selectedFile.files = state.selectedFile.files.filter(
          file => !(file.path === action.payload),
        );
      }
    },
    addSelectedDirectory(state, action: PayloadAction<string>) {
      if (!state.selectedFile.directories.includes(action.payload)) {
        state.selectedFile.directories.push(action.payload);
      }
    },
    updateSelectedDirectory(state, action: PayloadAction<string[]>) {
      state.selectedFile.directories = action.payload;
    },
    updateBackupFiles(state, action: PayloadAction<string[]>) {
      state.backupFiles = action.payload;
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
  updateSelectedDirectory,
  addSelectedFile,
  checkSelectedFile,
  checkSelectedDirectory,
  addSelectedDirectory,
} = CreateCommitReducer.actions;

export default CreateCommitReducer.reducer;
