
export interface BackUpFile {
  name: string;
  type: string;
  path: string;
}
export interface PostBackupRequest {
  files: BackUpFile[];
}

export interface PostBackupResponse {}

export function mock(request: PostBackupRequest): PostBackupResponse {
  return {};
}
