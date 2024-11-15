import { Notification } from "@/hooks/useApi/types";
export type PlainText = NotificationUnion & {
  type: "plainText";
  text: string;
  description?: string;
  state: "info" | "success" | "error" | "warning";
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

export enum CommitPushProgress {
  Cancel = 3,
  CollectingFiles = 0,
  PackingFiles = 1,
  Finish = 2,
}

export enum CommitRestoreProgress {
  Finish = 0,
  Cancel = 1,
}

export type CommitPush = NotificationUnion & {
  type: "commitPush";
  progress: CommitPushProgress;
  files: File[];
  directories: string[];
  options: BackupOption;
  result?: Notification;
};

export interface NotificationUnion {
  type: string;
  id: string;
}

export type CommitRestore = NotificationUnion & {
  type: "commitRestore";
  progress: CommitRestoreProgress;
  commitId: string;
  isEncrypt: boolean;
  result?: Notification;
};

