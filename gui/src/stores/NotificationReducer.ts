import {
  CommitPush,
  CommitRestore,
  NotificationUnion,
  PlainText,
} from "@/components/NotificationList/types";
import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { v4 as uuidv4 } from "uuid";
import { Notification } from "@/hooks/useApi/types";

export interface NotificationState {
  notifications: NotificationUnion[];
  open: boolean;
}

const initialState: NotificationState = {
  open: false,
  notifications: [
    {
      id: "1",
      type: "plainText",
      text: "Welcome to the notification center",
      description:
        "This is a notification center This is a notification centerThis is a notification centerThis is a notification centerThis is a notification center",
    } as PlainText,
  ],
};

const NotificationReducer = createSlice({
  name: "notification",
  initialState,
  reducers: {
    addNotification(state, action: PayloadAction<Omit<NotificationUnion, "id">>) {
      const id = uuidv4();
      state.notifications.unshift({ ...action.payload, id });
      state.open = true;
    },
    removeNotification(state, action: PayloadAction<string>) {
      state.notifications = state.notifications.filter(
        notification => notification.id !== action.payload,
      );
    },
    toggleNotification(state) {
      state.open = !state.open;
    },
    finishMessage(state, action: PayloadAction<Notification>) {
      const notification = state.notifications.find(
        notification => notification.id === action.payload.payload.id,
      );
      if (notification && notification.type === "commitPush") {
        (notification as CommitPush).result = action.payload;
      }
      if (notification && notification.type === "commitRestore") {
        (notification as CommitRestore).result = action.payload;
      }
    },
  },
});

export const {
  addNotification,
  removeNotification,
  toggleNotification,
  finishMessage,
} = NotificationReducer.actions;

export default NotificationReducer.reducer;
