import { FileType } from "@/types";

export interface FileDetail {
  name: string;
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}

export interface RootFileDetail {
  name: string;
  type: FileType.Directory;
  children: MockFileDetail[];
}

export interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  storagePath: string;
  author: string;
  isEncrypt: boolean;
  isDelete: boolean;
}

export type Filter = Partial<{
  type: FileType;
  name: string;
  minSize: number;
  maxSize: number;
  minCreateTime: number;
  maxCreateTime: number;
  minUpdateTime: number;
  maxUpdateTime: number;
  owner: string;
  group: string;
}>;

export interface MockFileDetail extends FileDetail {
  children: MockFileDetail[] | null;
}

type Config = {
  ip: string; // 服务器ip
  defaultBackupPath: string; // 默认备份路径
  port: number; // 端口
  version: string; // 版本
};
