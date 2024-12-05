import {
  CommitPush,
  CommitRestore,
  NotificationUnion,
  PlainText,
} from "@/components/NotificationList/types";
import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { v4 as uuidv4 } from "uuid";
import { BackupAndRestoreEnd, Notification } from "@/hooks/useApi/types";

export interface NotificationState {
  notifications: NotificationUnion[];
  open: boolean;
}

const initialState: NotificationState = {
  open: true,
  notifications: [
    {
      id: "1",
      state: "info",
      type: "plainText",
      text: "欢迎来到通知中心",
      description: "这是一个通知中心，为您提供各类重要通知。",
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
      state.open = false;
    },
    removeNotification(state, action: PayloadAction<string>) {
      state.notifications = state.notifications.filter(
        notification => notification.id !== action.payload,
      );
    },
    toggleNotification(state) {
      state.open = !state.open;
    },
    finishMessage(state, action: PayloadAction<Notification<BackupAndRestoreEnd>>) {
      const notification = state.notifications.find(
        notification => notification.id === action.payload.payload.messageId,
      );
      if (notification && notification.type === "commitPush") {
        (notification as CommitPush).result = action.payload;
      }
      if (notification && notification.type === "commitRestore") {
        (notification as CommitRestore).result = action.payload;
      }
    },
    ReportError(state, action: PayloadAction<Omit<PlainText, "id" | "type">>) {
      state.notifications.unshift({ ...action.payload, id: uuidv4(), type: "plainText" });
      state.open = false;
    },
  },
});

export const {
  addNotification,
  removeNotification,
  toggleNotification,
  ReportError,
  finishMessage,
} = NotificationReducer.actions;

export default NotificationReducer.reducer;
