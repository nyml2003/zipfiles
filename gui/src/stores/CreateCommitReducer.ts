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
    addSelectedFile(state, action: PayloadAction<File>) {
      if (
        !state.selectedFile.files.find(
          file => file.path === action.payload.path && file.name === action.payload.name,
        )
      ) {
        state.selectedFile.files.push(action.payload);
      }
    },
    removeSelectedFile(state, action: PayloadAction<File>) {
      state.selectedFile.files = state.selectedFile.files.filter(
        file => file.path !== action.payload.path || file.name !== action.payload.name,
      );
    },
    addSelectedDirectory(state, action: PayloadAction<string>) {
      const newDirectories = [];
      const newFiles = [];
      for (const directory of state.selectedFile.directories) {
        if (!directory.startsWith(action.payload)) {
          newDirectories.push(directory);
        }
      }
      for (const file of state.selectedFile.files) {
        if (!file.path.startsWith(action.payload)) {
          newFiles.push(file);
        }
      }
      newDirectories.push(action.payload);
      state.selectedFile.files = newFiles;
      state.selectedFile.directories = newDirectories;
    },
    removeSelectedDirectory(state, action: PayloadAction<string>) {
      state.selectedFile.directories = state.selectedFile.directories.filter(
        directory => directory !== action.payload,
      );
    },
    clearSelectedFiles(state) {
      state.selectedFile.files = [];
    },
    clearSelectedDirectories(state) {
      state.selectedFile.directories = [];
    },
    updateBackupFiles(state, action: PayloadAction<string[]>) {
      state.backupFiles = action.payload;
    },
  },
});

export const {
  updateCurrentFile,
  updateCurrentPath,
  handleRefresh,
  updateFilter,
  resetFilter,
  updateIsFiltering,
  addSelectedFile,
  removeSelectedFile,
  updateBackupFiles,
  removeSelectedDirectory,
  addSelectedDirectory,
  clearSelectedFiles,
  clearSelectedDirectories,
} = CreateCommitReducer.actions;

export default CreateCommitReducer.reducer;
