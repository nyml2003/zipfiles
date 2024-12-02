import { ApiEnum } from "@/apis";
import { GetFileDetailRequest, GetFileDetailResponse } from "@/apis/GetFileDetail";
import useApi from "@useApi";
import {
  clearSelectedDirectories,
  clearSelectedFiles,
  removeSelectedDirectory,
} from "@/stores/CreateCommitReducer";
import { RootState } from "@/stores/store";
import { FileType } from "@/types";
import { Button, Table } from "antd";
import React, { useEffect } from "react";
import { useDispatch, useSelector } from "react-redux";
import { ReportError } from "@/stores/NotificationReducer";

const columns = [
  {
    title: "文件名",
    dataIndex: "name",
    key: "name",
  },
  {
    title: "类型",
    dataIndex: "type",
    key: "type",
  },
  {
    title: "创建时间",
    dataIndex: "createTime",
    key: "createTime",
  },
  {
    title: "更新时间",
    dataIndex: "updateTime",
    key: "updateTime",
  },
  {
    title: "大小",
    dataIndex: "size",
    key: "size",
  },
  {
    title: "所有者",
    dataIndex: "owner",
    key: "owner",
  },
  {
    title: "组",
    dataIndex: "group",
    key: "group",
  },
  {
    title: "权限",
    dataIndex: "mode",
    key: "mode",
  },
];

interface FileDetail {
  name: string;
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}

interface FileListProps {
  addExplorer: () => void;
}

const FileList: React.FC<FileListProps> = ({ addExplorer }) => {
  const files = useSelector((state: RootState) => state.createCommit.selectedFile.files);
  const directories = useSelector(
    (state: RootState) => state.createCommit.selectedFile.directories,
  );
  const [fileData, setFileData] = React.useState<FileDetail[]>([]);
  const dispatch = useDispatch();

  const api = useApi();

  useEffect(() => {
    if (files.length === 0) {
      setFileData([]);
      return;
    }
    Promise.all(
      files.map(async (file: { name: string; path: string }) => {
        return await fetchFileDetail(file.path, file.name);
      }),
    )
      .then(res => {
        setFileData(res);
      })
      .catch(e => {
        dispatch(
          ReportError({
            state: "error",
            text: "获取文件详情失败",
            description: (e as Error).message,
          }),
        );
      });
  }, [files]);

  const handleAdd = () => {
    addExplorer();
  };

  const handleDelete = (record: string) => {
    dispatch(removeSelectedDirectory(record));
  };

  const fetchFileDetail = async (path: string, name: string) => {
    return await api.request<GetFileDetailRequest, GetFileDetailResponse>(ApiEnum.GetFileDetail, {
      path,
      name,
    });
  };

  const handleFileClear = () => {
    dispatch(clearSelectedFiles());
  };

  const handleDirectoryClear = () => {
    dispatch(clearSelectedDirectories());
  };

  const directoryColumns = [
    {
      title: "目录名",
      dataIndex: "name",
      key: "name",
      render: (_: string, record: string) => <span>{record ? record : "/"}</span>,
    },
    {
      title: "操作",
      key: "action",
      render: (_: string, record: string) => (
        <Button type='link' onClick={() => handleDelete(record)}>
          删除
        </Button>
      ),
    },
  ];

  return (
    <div className='grow-item'>
      <div className='rounded-md p-2 m-2 bg-white shadow-md'>
        <div className='flex flex-row justify-between items-center border-b border-gray-200 pb-2'>
          <h2 className='text-xl font-bold px-2 py-1 m-0'>文件列表</h2>
          <Button
            type='primary'
            onClick={handleFileClear}
            className='m-2'
            disabled={fileData.length === 0}>
            清空
          </Button>
        </div>
        <Table<FileDetail>
          columns={columns}
          dataSource={fileData}
          size='small'
          rowKey={"name"}
          className='p-2'
        />
      </div>
      <div className='m-4 flex flex-col text-gray-600'>
        <span>仅备份目录文件信息时，不会备份目录下的文件</span>
        <span>若目录文件出现在文件列表中，说明是仅备份目录文件信息</span>
      </div>
      <div className='rounded-md p-2 m-2 bg-white shadow-md'>
        <div className='flex flex-row justify-between items-center border-b border-gray-200 pb-2'>
          <h2 className='text-xl font-bold px-2 py-1 m-0'>目录列表</h2>
          <Button
            type='primary'
            onClick={handleDirectoryClear}
            className='m-2'
            disabled={directories.length === 0}>
            清空
          </Button>
        </div>
        <Table<string>
          columns={directoryColumns}
          dataSource={directories}
          size='small'
          rowKey={record => record}
          className='p-2'
        />
      </div>
      <div className='m-4 flex flex-col text-gray-600'>
        <span>出现在目录列表中的目录，会先查询目录下的所有文件</span>
      </div>
      <div className='m-4 flex flex-row justify-end items-center'>
        <div className='space-x-2'>
          <Button type='primary' onClick={handleAdd}>
            添加
          </Button>
        </div>
      </div>
    </div>
  );
};

export default FileList;
