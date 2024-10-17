/* eslint-disable no-unused-vars */
export interface RouteConfig {
  path: string;
  element: React.LazyExoticComponent<() => React.JSX.Element>;
  children?: () => Promise<RouteConfig[]>;
}
export enum DarkMode {
  Light = 'light',
  Dark = 'dark',
}

export class IngoreError extends Error {
  constructor() {
    super('IngoreError');
  }
}
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
      return 'None';
    case FileType.NotFound:
      return 'NotFound';
    case FileType.Regular:
      return 'Regular';
    case FileType.Directory:
      return 'Directory';
    case FileType.Symlink:
      return 'Symlink';
    case FileType.Block:
      return 'Block';
    case FileType.Character:
      return 'Character';
    case FileType.Fifo:
      return 'Fifo';
    case FileType.Socket:
      return 'Socket';
    case FileType.Unknown:
      return 'Unknown';
    default:
      throw new Error(`Unknown FileType: ${type}`);
  }
}

export interface Filter {
  type: FileType;
  size: {
    min: number;
    max: number;
  };
  owner: string;
  group: string;
}

export interface NestedFileDetail {
  name: string;
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
  children: NestedFileDetail[] | null;
}
