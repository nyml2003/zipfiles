// store.ts
import { configureStore } from '@reduxjs/toolkit';
import counterReducer from './counter/reducer';
import fileReducer from './file/reducer';
import CommitListReducer from './CommitListReducer';

const store = configureStore({
  reducer: {
    counter: counterReducer,
    file: fileReducer,
    commitList: CommitListReducer,
  },
});

export type RootState = ReturnType<typeof store.getState>;

export default store;
