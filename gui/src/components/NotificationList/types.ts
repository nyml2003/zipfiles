import { BackupAndRestoreEnd, Notification } from "@/hooks/useApi/types";
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

export type CommitPush = NotificationUnion & {
  type: "commitPush";
  files: File[];
  directories: string[];
  options: BackupOption;
  result?: Notification<BackupAndRestoreEnd>;
};

export type CommitRestore = NotificationUnion & {
  type: "commitRestore";
  commitId: string;
  path: string;
  password: string;
  result?: Notification<BackupAndRestoreEnd>;
};

export interface NotificationUnion {
  type: string;
  id: string;
}

