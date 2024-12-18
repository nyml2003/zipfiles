import { BackupAndRestoreEnd, Notification } from "@/hooks/useApi/types";

// Notification的父接口
export interface NotificationUnion {
  type: string;
  id: string;
} 

// 普通文本通知，包含状态、文本、描述
export type PlainText = NotificationUnion & {
  type: "plainText";
  text: string;
  description?: string;
  state: "info" | "success" | "error" | "warning";
};

// 文件类型
type File = {
  name: string;
  path: string;
};

// 备份选项
type BackupOption = {
  message: string;
  storagePath: string;
  isEncrypt: boolean;
  author: string;
  key?: string;
};

// 渲染备份文件需要的数据
export type CommitPush = NotificationUnion & {
  type: "commitPush";
  files: File[];
  directories: {
    name: string;
  }[];
  options: BackupOption;
  result?: Notification<BackupAndRestoreEnd>;
};

// 渲染还原备份需要的数据
export type CommitRestore = NotificationUnion & {
  type: "commitRestore";
  commitId: string;
  path: string;
  password: string;
  result?: Notification<BackupAndRestoreEnd>;
};

