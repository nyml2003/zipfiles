import { GetCommitListResponse } from "../GetCommitList";
import { cachedCommitList } from "./init";

export function mock(): GetCommitListResponse {
  return {
    commits: cachedCommitList.filter(commit => !commit.isDelete)
  };
}
