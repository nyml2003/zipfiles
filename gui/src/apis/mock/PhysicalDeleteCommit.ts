import {
  PhysicalDeleteCommitRequest,
  PhysicalDeleteCommitResponse
} from "@/apis/PhysicalDeleteCommit";
import { cachedCommitList } from "./init";
export function mock(request: PhysicalDeleteCommitRequest): PhysicalDeleteCommitResponse {
  const commitId = request.commitId;
  cachedCommitList.forEach((commit, index) => {
    if (commit.uuid === commitId) {
      cachedCommitList.splice(index, 1);
    }
  });
  return {};
}
