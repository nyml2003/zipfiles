import { LogicDeleteCommitRequest, LogicDeleteCommitResponse } from "@/apis/LogicDeleteCommit";
import { cachedCommitList } from "./init";
export function mock(request: LogicDeleteCommitRequest): LogicDeleteCommitResponse {
  const commitId = request.commitId;
  cachedCommitList.forEach((commit, index) => {
    if (commit.uuid === commitId) {
      cachedCommitList[index].isDelete = true;
    }
  });
  return {};
}
