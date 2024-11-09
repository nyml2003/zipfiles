import {
  CommitPush,
  NotificationUnion,
  PlainText,
  Progress,
} from '@/components/NotificationList/types';
import { createSlice, PayloadAction } from '@reduxjs/toolkit';
import { v4 as uuidv4 } from 'uuid';
import { Notification } from '@/hooks/useApi/types';

export interface NotificationState {
  notifications: NotificationUnion[];
  open: boolean;
}

const initialState: NotificationState = {
  open: false,
  notifications: [],
};

const NotificationReducer = createSlice({
  name: 'notification',
  initialState,
  reducers: {
    addNotification(state, action: PayloadAction<Omit<NotificationUnion, 'id'>>) {
      const id = uuidv4();
      state.notifications.unshift({ ...action.payload, id });
    },
    removeNotification(state, action: PayloadAction<string>) {
      state.notifications = state.notifications.filter(
        notification => notification.id !== action.payload,
      );
    },
    updateProgress(state, action: PayloadAction<{ id: string; progress: Progress }>) {
      const notification = state.notifications.find(
        notification => notification.id === action.payload.id,
      );
      if (notification && notification.type === 'commitPush') {
        (notification as CommitPush).progress = action.payload.progress;
      }
    },
    openNotification(state) {
      state.open = true;
    },
    closeNotification(state) {
      state.open = false;
    },
    toggleNotification(state) {
      state.open = !state.open;
    },
    finishMessage(state, action: PayloadAction<Notification>) {
      console.log('finishMessage', JSON.stringify(action.payload));
      const notification = state.notifications.find(
        notification => notification.id === action.payload.payload.id,
      );
      console.log('notifications', JSON.stringify(state.notifications));
      console.log('notification', JSON.stringify(notification));
      if (notification && notification.type === 'commitPush') {
        (notification as CommitPush).result = action.payload;
      }
    },
  },
});

export const {
  addNotification,
  removeNotification,
  updateProgress,
  openNotification,
  closeNotification,
  toggleNotification,
  finishMessage,
} = NotificationReducer.actions;

export default NotificationReducer.reducer;
