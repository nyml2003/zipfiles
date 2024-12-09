import React, { useEffect, useState } from "react";
import { CommitPush as CommitPushProps } from "./types";
import { FileType } from "@/types";
import { GetFileListRequest, GetFileListResponse } from "@/apis/GetFileList";
import { ApiEnum } from "@/apis";
import useApi from "@useApi";
import { PostCommitRequest, PostCommitResponse } from "@/apis/PostCommit";
import { Step, StepProps } from "@/components/Step/Step";
import Button from "@/components/Button";
import { AcceptableError, Code } from "@/hooks/useApi/types";
import { useDispatch } from "react-redux";
import { ReportError } from "@/stores/NotificationReducer";
interface File {
  name: string;
  type: FileType;
}

const CommitPush = ({ files, directories, options, id, result }: CommitPushProps) => {
  const dispatch = useDispatch();
  const api = useApi();
  const fetchFileList = async (path: string) => {
    try {
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path
      });
      return res.files;
    } catch (e: unknown) {
      if (!(e instanceof AcceptableError)) {
        return [];
      }
      dispatch(
        ReportError({
          state: "error",
          text: "获取文件列表失败",
          description: (e as Error).message
        })
      );
    }
    return [];
  };
  /**
   * @brief 返回指定路径下的所有文件
   * @description 先获取指定路径下的一级文件，如果该文件是文件，直接加入集合，否则调用fetchAllFiles后，concat在后面
   * @param path
   * @returns String[] 所有文件的path+name形成的path的集合
   */
  const fetchAllFiles = async (path: string): Promise<string[]> => {
    const files = await fetchFileList(path);
    const allFiles: string[] = [];
    const promises: Promise<void>[] = files.map(async (file: File) => {
      allFiles.push(path + "/" + file.name);
      if (file.type === FileType.Directory) {
        const subFiles = await fetchAllFiles(path + "/" + file.name);
        allFiles.push(...subFiles);
      }
      return Promise.resolve();
    });
    await Promise.all(promises);
    return allFiles;
  };
  const collectFiles = async () => {
    const fileData = files.map(file => file.path + "/" + file.name);
    let dirData: string[] = [];
    const dirPromises = directories.map(async path => {
      return await fetchAllFiles(path.name);
    });
    const dirResults = await Promise.all(dirPromises);
    dirData = dirResults.flat(); // 将所有目录的结果合并到 dirData
    return [...fileData, ...dirData];
  };
  useEffect(() => {
    if (!result) return;
    if (result.code === Code.BACKUP_SUCCESS) {
      setPackingFiles(prev => {
        return {
          ...prev,
          status: "completed",
          description: "备份完成"
        };
      });
    }
    if (result.code === Code.BACKUP_FAILED) {
      setPackingFiles(prev => {
        return {
          ...prev,
          status: "failed",
          description: result.payload.description
        };
      });
    }
  }, [result]);
  const [collectingFiles, setCollectingFiles] = useState<StepProps>({
    title: "统计",
    status: "pending"
  });
  const [packingFiles, setPackingFiles] = useState<StepProps>({
    title: "备份",
    status: "pending"
  });
  const execute = async () => {
    setStart(true);
    setCollectingFiles(prev => {
      return {
        ...prev,
        status: "running",
        description: "正在收集文件"
      };
    });
    const backupFiles = await collectFiles();
    const request: PostCommitRequest = {
      files: backupFiles,
      ...options,
      uuid: id,
      createTime: Date.now()
    };
    setCollectingFiles(prev => {
      return {
        ...prev,
        status: "completed",
        description: (
          <div>
            <div>备份信息如下, 正在打包中</div>
            <div>备份的uuid为: {id}</div>
            <div>备份的文件数为: {backupFiles.length}</div>
            <div>备份的信息为: {options.message}</div>
            <div>备份的作者为: {options.author}</div>
            <div>备份的路径为: {options.storagePath}</div>
            <div>提交的时间为: {request.createTime}</div>
            <span>是否加密: {options.isEncrypt ? <span style={{ color: "green" }}>√</span> : <span style={{ color: "red" }}>×</span>}</span>
          </div>
        )
      };
    });
    setPackingFiles(prev => {
      return {
        ...prev,
        status: "running",
        description: "发送文件中"
      };
    });
    try {
      await api.request<PostCommitRequest, PostCommitResponse>(ApiEnum.PostCommit, request);
    } catch (e) {
      setPackingFiles(prev => {
        return {
          ...prev,
          status: "failed",
          description: (e as Error).message
        };
      });
      return;
    }
    setPackingFiles(prev => {
      return {
        ...prev,
        description: "备份中"
      };
    });
  };

  const [start, setStart] = useState<boolean>(false);
  return (
    <div className='grow-item split-container-col div-2'>
      <Step {...collectingFiles} />
      <Step {...packingFiles} />

      <div className='flex space-x-2 justify-end'>
        <Button onClick={() => execute()} disabled={start} variant='confirm' visible={!start}>
          开始执行
        </Button>
      </div>
    </div>
  );
};

export default CommitPush;
