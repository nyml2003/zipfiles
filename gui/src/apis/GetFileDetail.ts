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
export interface GetFileDetailRequest {
  path: string;
  name: string;
}

export interface GetFileDetailResponse extends FileDetail { }

