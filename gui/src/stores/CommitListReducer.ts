import { createSlice, PayloadAction } from '@reduxjs/toolkit';

export interface CommitListState {
    rootPath: string;
    currentPath: string;
}

const initialState: CommitListState = {
    rootPath: '',
    currentPath: '',
};

const commitListSlice = createSlice({
    name: 'commitList',
    initialState,
    reducers: {
        updateRootPath(state, action: PayloadAction<string>) {
            state.rootPath = action.payload;
        },
        updateCurrentPath(state, action: PayloadAction<string>) {
            state.currentPath = action.payload;
        },
    },
});

export const { updateRootPath, updateCurrentPath } = commitListSlice.actions;

export default commitListSlice.reducer;
