export enum LoadingState {
  Loading,
  Done,
}

export enum FileType {
  None = 0,
  NotFound = -1,
  Regular = 1,
  Directory = 2,
  Symlink = 3,
  Block = 4,
  Character = 5,
  Fifo = 6,
  Socket = 7,
  Unknown = 8,
}

export function FileTypeToString(type: FileType): string {
  switch (type) {
    case FileType.None:
      return "空";
    case FileType.NotFound:
      return "未找到";
    case FileType.Regular:
      return "文件";
    case FileType.Directory:
      return "目录";
    case FileType.Symlink:
      return "符号链接";
    case FileType.Block:
      return "块设备";
    case FileType.Character:
      return "字符设备";
    case FileType.Fifo:
      return "管道";
    case FileType.Socket:
      return "套接字";
    case FileType.Unknown:
      return "未知";
    default:
      return "Undefined";
  }
}
