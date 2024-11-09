import { Notification } from '@/hooks/useApi/types';
export type PlainText = NotificationUnion & {
  type: 'plainText';
  text: string;
  description?: string;
  state: 'info' | 'success' | 'error' | 'warning';
};

type File = {
  name: string;
  path: string;
};

type BackupOption = {
  message: string;
  storagePath: string;
  isEncrypt: boolean;
  author: string;
  key?: string;
};

export enum Progress {
  Cancel = 3,
  CollectingFiles = 0,
  PackingFiles = 1,
  Finish = 2,
}

export type CommitPush = NotificationUnion & {
  type: 'commitPush';
  progress: Progress;
  files: File[];
  directories: string[];
  options: BackupOption;
  result?: Notification;
};

export interface NotificationUnion {
  type: string;
  id: string;
}
