import React, { useEffect, useState } from "react";
import { CommitRestore as CommitRestoreProps } from "./types";
import Button from "@/components/Button";
import { Code } from "@/hooks/useApi/types";
import { ApiEnum } from "@/apis";
import useApi from "@useApi";
import { RestoreRequest, RestoreResponse } from "@/apis/Restore";
import { Step, StepProps } from "../Step/Step";
const RestoreInfo = ({
  commitId,
  path,
}: {
  commitId: string;
  path: string;
}) => {
  return (
    <div>
      <div className='text-lg font-semibold mb-2'>请确认还原信息</div>
      <div className='mb-2'>
        <label htmlFor='commit-id' className='block text-sm font-medium text-gray-600 mb-1'>
          Commit ID:
        </label>
        <div id='commit-id' className='bg-gray-100 rounded px-2 py-1 text-sm font-semibold'>
          {commitId}
        </div>
      </div>
      <div className='mb-2'>
        <label htmlFor='path' className='block text-sm font-medium text-gray-600 mb-1'>
          Path:
        </label>
        <div id='path' className='bg-gray-100 rounded px-2 py-1 text-sm font-semibold'>
          {path}
        </div>
      </div>
    </div>
  );
};
const CommitRestore = ({ password, id, result, commitId, path }: CommitRestoreProps) => {
  const api = useApi();
  const [confirmRequest, setConfirmRequest] = useState<StepProps>({
    title: "确认请求",
    status: "running",
    description: RestoreInfo({ commitId, path}),
  });

  const [restoreRequest, setRestoreRequest] = useState<StepProps>({
    title: "还原",
    status: "pending",
  });

  const restore = async () => {
    const key = password ? { key: password } : {};
    setRestoreRequest(prev => {
      return { ...prev, status: "running", description: "正在发送还原请求" };
    });
    try {
      await api.request<RestoreRequest, RestoreResponse>(ApiEnum.Restore, {
        commitId,
        path,
        messageId: id,
        ...key,
      });
    } catch (e) {
      setRestoreRequest(prev => {
        return { ...prev, status: "failed", description: (e as Error).message };
      });
    }
    setRestoreRequest(prev => {
      return { ...prev, description: "还原中" };
    });
  };

  useEffect(() => {
    if (!result) return;
    if (result.code === Code.RESTORE_SUCCESS) {
      setRestoreRequest(prev => {
        return { ...prev, status: "completed", description: "还原完成" };
      });
    }
    if (result.code === Code.RESTORE_FAILED) {
      setRestoreRequest(prev => {
        return { ...prev, status: "failed", description: result.message };
      });
    }
  }, [result]);

  const execute = async () => {
    setStart(true);
    setConfirmRequest(prev => {
      return { ...prev, status: "completed" };
    });
    await restore();
  };
  const [start, setStart] = useState<boolean>(false);
  return (
    <div className='grow-item split-container-col div-2'>
      <Step {...confirmRequest} />
      <Step {...restoreRequest} />

      <div className='flex space-x-2 justify-end'>
        <Button onClick={() => execute()} disabled={start} variant='confirm' visible={!start}>
          开始执行
        </Button>
      </div>
    </div>
  );
};

export default CommitRestore;
