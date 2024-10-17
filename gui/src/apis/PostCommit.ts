export interface PostCommitRequest {
  // 在新标签页中选取
  files: string[]; // path

  // 请求时创建
  uuid: string;
  createTime: number;
  // 请求时计算
  lca: string; // lca is the lowest common ancestor of all files

  // 表单中填写
  message: string;
  storagePath: string;
  isEncrypt: boolean;
  author: string;
  key?: string; // 当isEncrypt为true时，key必填
}

export interface PostCommitResponse {}
