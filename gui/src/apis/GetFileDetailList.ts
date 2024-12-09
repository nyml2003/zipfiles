import { FileType } from "@/types";
interface FileDetail {
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

type Filter = Partial<{
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

export interface GetFileDetailListRequest {
  path: string;
  filter?: Filter;
}

export interface GetFileDetailListResponse{
  files: FileDetail[];
}
