import { FileType } from "@/types";

export function selectEnvironment<T>(production: T, development: T) {
  if (process.env.BASE_ENV === 'prod') {
    return production;
  } else {
    return development;
  }
}

export function pickIndex(choices: any[], _weights?: number[]) {
  if (!_weights) {
    return Math.floor(Math.random() * choices.length);
  }
  const weights = _weights;
  const x = Math.floor(Math.random() * weights.reduce((a, b) => a + b));
  const pre = weights.reduce((acc, cur, i) => {
    acc.push(i === 0 ? cur : acc[i - 1] + cur);
    return acc;
  }, [] as number[]);
  const binarySearch = (x: number) => {
    let low = 0,
      high = pre.length - 1;
    while (low < high) {
      const mid = Math.floor((high - low) / 2) + low;
      if (pre[mid] < x) {
        low = mid + 1;
      } else {
        high = mid;
      }
    }
    return low;
  };
  return binarySearch(x);
}

export function pickFileType() {
  const fileTypes = [
    FileType.None,
    FileType.NotFound,
    FileType.Regular,
    FileType.Directory,
    FileType.Symlink,
    FileType.Block,
    FileType.Character,
    FileType.Fifo,
    FileType.Socket,
    FileType.Unknown,
  ];
  const weights = [1, 1, 5, 3, 2, 1, 1, 1, 1, 1];
  return pickIndex(fileTypes, weights);
}