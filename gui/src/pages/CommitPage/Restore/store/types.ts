export interface State {
  path: string; // 当前路径
  commitId: string; // commitId
  isEncrypt: boolean; // 是否加密
  chosenCommitId: string; // 选中的commitId
  fresh: boolean; // 是否刷新
}
