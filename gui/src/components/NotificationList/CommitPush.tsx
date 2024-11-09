import React, { useEffect, useState } from 'react';
import { CommitPush as CommitPushProps, Progress } from './types';
import { Button, message, Space, Steps, notification } from 'antd';
import { FileType } from '@/types';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { ApiEnum } from '@/apis';
import useApi from '@useApi';
import { PostCommitRequest, PostCommitResponse } from '@/apis/PostCommit';
import { v4 as uuidv4 } from 'uuid';
import { useDispatch } from 'react-redux';
import { updateProgress } from '@/stores/NotificationReducer';
import { StepsProps } from 'antd/lib';
import { CheckCircleOutlined, LoadingOutlined, StopOutlined } from '@ant-design/icons';
import { Code } from '@/hooks/useApi/types';

interface File {
  name: string;
  type: FileType;
}

const progresses: Progress[] = [Progress.CollectingFiles, Progress.PackingFiles, Progress.Finish];

const CommitPush = ({ progress, files, directories, options, id, result }: CommitPushProps) => {
  const dispatch = useDispatch();
  const [backupFiles, setBackupFiles] = useState<string[]>([]);

  const api = useApi();
  const fetchFileList = async (path: string) => {
    const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
      path,
    });
    return res.files;
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
    const promises: Promise<void>[] = [];

    files.forEach((file: File) => {
      if (file.type === FileType.Directory) {
        promises.push(
          fetchAllFiles(path + '/' + file.name).then(subFiles => {
            allFiles.push(...subFiles);
          }),
        );
      }
      allFiles.push(path + '/' + file.name);
    });

    await Promise.all(promises);
    return allFiles;
  };

  const [steps, setSteps] = useState<StepsProps['items']>([
    {
      title: '收集文件',
    },
    {
      title: '打包文件',
    },
    {
      title: '完成',
      description: '提交完成',
    },
  ]);

  const collectFiles = async () => {
    setSteps(prev => {
      if (!prev) return prev;
      prev[0].description = '正在收集文件';
      prev[0].status = 'process';
      prev[0].icon = <LoadingOutlined />;
      return [...prev];
    });
    const fileData = files.map(file => file.path + '/' + file.name);
    let dirData: string[] = [];
    const dirPromises = directories.map(async path => {
      const files = await fetchAllFiles(path);
      return files;
    });
    const dirResults = await Promise.all(dirPromises);
    dirData = dirResults.flat(); // 将所有目录的结果合并到 dirData
    setBackupFiles([...fileData, ...dirData]);
    setSteps(prev => {
      if (!prev) return prev;
      prev[0].description = `共计${fileData.length + dirData.length}个文件`;
      prev[0].status = 'finish';
      prev[0].icon = <CheckCircleOutlined />;
      return [...prev];
    });
  };

  const backup = async () => {
    setSteps(prev => {
      if (!prev) return prev;
      prev[1].description = '正在发送文件列表';
      prev[1].status = 'process';
      prev[1].icon = <LoadingOutlined />;
      return [...prev];
    });
    const request: PostCommitRequest = {
      files: backupFiles,
      ...options,
      uuid: id,
      createTime: Date.now(),
    };
    await api.request<PostCommitRequest, PostCommitResponse>(ApiEnum.PostCommit, request);
    setSteps(prev => {
      if (!prev) return prev;
      prev[1].description = `本次提交的uuid为${id}, 文件正在备份中`;
      prev[1].status = 'finish';
      prev[1].icon = <CheckCircleOutlined />;
      prev[2].description = '备份中';
      prev[2].status = 'process';
      prev[2].icon = <LoadingOutlined />;
      return [...prev];
    });
  };

  const nextProgress = async () => {
    if (progress === Progress.CollectingFiles) {
      await collectFiles();
    } else if (progress === Progress.PackingFiles) {
      await backup();
    }
    dispatch(updateProgress({ id, progress: progresses[progress + 1] }));
  };

  const cancel = () => {
    dispatch(updateProgress({ id, progress: Progress.Cancel }));
  };

  useEffect(() => {
    if (!result) return;
    if (result.code === Code.POSTCOMMIT_SUCCESS) {
      setSteps(prev => {
        if (!prev) return prev;
        prev[2].description = '提交成功';
        prev[2].status = 'finish';
        prev[2].icon = <CheckCircleOutlined />;
        return [...prev];
      });
    }
    if (result.code === Code.POSTCOMMIT_FAILED) {
      setSteps(prev => {
        if (!prev) return prev;
        prev[2].description = '提交失败';
        prev[2].status = 'error';
        prev[2].icon = <StopOutlined />;
        return [...prev];
      });
    }
  }, [result]);

  return progress !== Progress.Cancel ? (
    <div>
      <Steps current={progress} size='small' direction='vertical' items={steps} />
      {progress !== Progress.Finish && (
        <div className='absolute bottom-2 right-2'>
          <Space>
            <Button type='text' size='small' onClick={cancel}>
              取消
            </Button>
            <Button type='primary' size='small' onClick={nextProgress}>
              继续
            </Button>
          </Space>
        </div>
      )}
    </div>
  ) : (
    <Steps
      current={0}
      size='small'
      direction='vertical'
      items={[
        {
          title: '取消',
          description: '取消提交',
          status: 'error',
        },
      ]}
    />
  );
};

export default CommitPush;