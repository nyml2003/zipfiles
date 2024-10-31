// store.ts
import { configureStore } from '@reduxjs/toolkit';
import counterReducer from './counter/reducer';
import CreateCommitReducer from './CreateCommitReducer';
// import CommitListReducer from './CommitListReducer';

const store = configureStore({
  reducer: {
    counter: counterReducer,
    createCommit: CreateCommitReducer,
    // commitList: CommitListReducer,
  },
});

export type RootState = ReturnType<typeof store.getState>;

export default store;
