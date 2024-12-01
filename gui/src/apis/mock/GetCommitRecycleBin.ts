
import { cachedCommitList } from "./init";
import { GetCommitRecycleBinResponse } from "../GetCommitRecycleBin";

export function mock(): GetCommitRecycleBinResponse {
  const commits = cachedCommitList.filter(commit => commit.isDelete);
  return { commits };
}
