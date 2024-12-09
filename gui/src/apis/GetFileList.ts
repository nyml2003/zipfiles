import { FileType } from "@/types";
interface File {
  name: string;
  type: FileType;
}
export interface GetFileListRequest {
  path: string;
}

export interface GetFileListResponse {
  files: File[];
}

