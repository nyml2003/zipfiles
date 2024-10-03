// store.ts
import { configureStore } from '@reduxjs/toolkit';
import counterReducer from './counter/reducer';
import fileReducer from './file/reducer';

const store = configureStore({
  reducer: {
    counter: counterReducer,
    file: fileReducer,
  },
});

export type RootState = ReturnType<typeof store.getState>;

export default store;
