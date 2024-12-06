import { configureStore } from "@reduxjs/toolkit";
import counterReducer from "./counter/reducer";
import CreateCommitReducer from "./CreateCommitReducer";
import NotificationReducer from "./NotificationReducer";
import SocketReducer from "./SocketReducer";

const store = configureStore({
  reducer: {
    counter: counterReducer,
    createCommit: CreateCommitReducer,
    notification: NotificationReducer,
    socket: SocketReducer,
  },
});

export type RootState = ReturnType<typeof store.getState>;

export default store;
