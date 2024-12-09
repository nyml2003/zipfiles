import { ApiEnum } from "@/apis";
import { GetFileDetailRequest, GetFileDetailResponse } from "@/apis/GetFileDetail";
import useApi from "@useApi";
import { clearSelectedDirectories, clearSelectedFiles } from "@/stores/CreateCommitReducer";
import { RootState } from "@/stores/store";
import { FileType, FileTypeToString } from "@/types";
import { Table, TableColumnType } from "antd";
import React, { useEffect } from "react";
import { useDispatch, useSelector } from "react-redux";
import { ReportError } from "@/stores/NotificationReducer";
import { AcceptableError } from "@/hooks/useApi/types";
import { convertBytesToHumanReadable, modeToString } from "@/utils";
import Button from "@/components/Button";

const columns: TableColumnType<DataType>[] = [
  {
    title: "文件名",
    dataIndex: "name",
    key: "name",
    render: (_: string, record: DataType) => {
      return record.name || "/";
    }
  },
  {
    title: "是否递归备份",
    render: (_: string, record: DataType) => (
      <span>
        {Object.hasOwnProperty.call(record, "path") ? (
          <div className='text-red-500'>×</div>
        ) : (
          <div className='text-green-500'>√</div>
        )}
      </span>
    )
  },
  {
    title: "类型",
    dataIndex: "type",
    ellipsis: true,
    key: "type",
    render: (text: FileType, record: DataType) => {
      if (!Object.hasOwnProperty.call(record, "path")) return <span>-</span>;
      return FileTypeToString(text);
    },
    width: 75
  },
  {
    title: "创建时间",
    dataIndex: "createTime",
    key: "createTime",
    ellipsis: true,
    render: (text: number, record: DataType) => {
      if (!Object.hasOwnProperty.call(record, "path")) return <span>-</span>;
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span>加载中...</span>;
    },
    width: 150
  },
  {
    title: "更新时间",
    dataIndex: "updateTime",
    key: "updateTime",
    ellipsis: true,
    render: (text: number, record: DataType) => {
      if (!Object.hasOwnProperty.call(record, "path")) return <span>-</span>;
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span>加载中...</span>;
    },
    width: 150
  },
  {
    title: "大小",
    dataIndex: "size",
    key: "size",
    ellipsis: true,
    render: (text: number, record: DataType) => {
      if (!Object.hasOwnProperty.call(record, "path")) return <span>-</span>;
      if (text === undefined) return <span>加载中...</span>;
      if (text === null) return <span>未知</span>;
      if (text === 0) return <span>-</span>;
      return <span>{convertBytesToHumanReadable(text)}</span>;
    },
    width: 100
  },
  {
    title: "所有者",
    dataIndex: "owner",
    key: "owner",
    ellipsis: true,
    render: (text: string, record: DataType) => {
      if (!Object.hasOwnProperty.call(record, "path")) return <span>-</span>;
      return text ? <span>{text}</span> : <span>加载中...</span>;
    }
  },
  {
    title: "组",
    dataIndex: "group",
    key: "group",
    ellipsis: true,
    render: (text: string, record: DataType) => {
      if (!Object.hasOwnProperty.call(record, "path")) return <span>-</span>;
      return text ? <span>{text}</span> : <span>加载中...</span>;
    }
  },
  {
    title: "权限",
    dataIndex: "mode",
    key: "mode",
    ellipsis: true,
    render: (text: number, record: DataType) => {
      if (!Object.hasOwnProperty.call(record, "path")) return <span>-</span>;
      return text ? <span>{modeToString(text)}</span> : <span>加载中...</span>;
    },
    width: 75,
    align: "center"
  }
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

type DataType = FileDetail | { name: string };

interface FileListProps {
  addExplorer: () => void;
}

const FileList: React.FC<FileListProps> = ({ addExplorer }) => {
  const files = useSelector((state: RootState) => state.createCommit.selectedFile.files);
  const directories = useSelector(
    (state: RootState) => state.createCommit.selectedFile.directories
  );
  const [fileData, setFileData] = React.useState<DataType[]>([]);
  const dispatch = useDispatch();

  const api = useApi();

  useEffect(() => {
    if (files.length === 0) {
      setFileData([...directories]);
      return;
    }
    Promise.all(
      files.map(async (file: { name: string; path: string }) => {
        return await fetchFileDetail(file.path, file.name);
      })
    )
      .then(res => {
        setFileData([...directories, ...res]);
      })
      .catch(e => {
        if (!(e instanceof AcceptableError)) {
          return;
        }
        dispatch(
          ReportError({
            state: "error",
            text: "获取文件详情失败",
            description: (e as Error).message
          })
        );
      });
  }, [files, directories]);

  const handleAdd = () => {
    addExplorer();
  };
  const fetchFileDetail = async (path: string, name: string) => {
    return await api.request<GetFileDetailRequest, GetFileDetailResponse>(ApiEnum.GetFileDetail, {
      path,
      name
    });
  };

  const handleFileClear = () => {
    dispatch(clearSelectedFiles());
    dispatch(clearSelectedDirectories());
  };

  return (
    <div className='grow-item'>
      <div className='flex flex-col justify-between items-center border-gray-200 p-2'>
        <div className='flex justify-between w-full'>
          <div className='text-xl font-bold px-2 py-1'>文件列表</div>
          <div className='space-x-2'>
            <Button variant='confirm' onClick={handleAdd}>
              添加
            </Button>
            <Button variant='primary' onClick={handleFileClear} disabled={fileData.length === 0}>
              清空
            </Button>
          </div>
        </div>
        {fileData.length !== 0 && (
          <Table<DataType>
            columns={columns}
            dataSource={fileData}
            size='small'
            rowKey={"name"}
            className='grow-item w-full mt-2'
            scroll={{ x: "max-content" }}
          />
        )}
      </div>
      <div className='m-4 flex flex-col text-gray-600'>
        <span>提示：</span>
        <span>1. 文件列表中的文件将会被添加到备份中</span>
        <span>
          2. 递归备份列显示为<span className='text-green-500'>√</span>的目录将会被递归备份
        </span>
      </div>
    </div>
  );
};

export default FileList;
