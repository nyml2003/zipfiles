import { RecoverCommitRequest, RecoverCommitResponse } from "../RecoverCommit";
import { cachedCommitList } from "./init";

export function mock(request: RecoverCommitRequest): RecoverCommitResponse {
  const commitId = request.commitId;
  cachedCommitList.forEach((commit, index) => {
      if (commit.uuid === commitId) {
        if (commit.isDelete) {
          commit.isDelete = false;
        } else {
            throw new Error("commit is not in recycle bin");
        }
    }
  });
  return {};
}
