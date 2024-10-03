import React, { useEffect, useState } from 'react';
import { Table } from 'antd';
import type { TableColumnsType } from 'antd';
import { FileDetail } from '@/apis/GetFileDetail';
import { File, GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import useApi from '@/hooks/useApi';
import { ApiEnum } from '@/apis';
import { FileType, FileTypeToString } from '@/types';
import { FileFilled, FolderFilled } from '@ant-design/icons';
import { GetAllFileDetailsRequest, GetAllFileDetailsResponse } from '@/apis/GetAllFileDetails';
import styles from './index.module.less';
import { useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { handleRefresh } from '@/stores/file/reducer';

interface DataType extends Partial<FileDetail> {}

const columns: TableColumnsType<DataType> = [
  {
    title: '文件名',
    dataIndex: 'name',
    key: 'name',
    render: (text, record) => {
      return <a>{text}</a>;
    },
  },
  {
    title: '类型',
    dataIndex: 'type',
    key: 'type',
    render: (text, record) => {
      if (text === FileType.Directory) {
        return <FolderFilled />;
      }
      if (text === FileType.Regular) {
        return <FileFilled />;
      }
      return FileTypeToString(text);
    },
  },
  {
    title: '创建时间',
    dataIndex: 'createTime',
    key: 'createTime',
    render: (text, record) => {
      return text ? (
        <span>{new Date(text * 1000).toLocaleString()}</span>
      ) : (
        <span>加载中...</span>
      );
    },
  },
  {
    title: '更新时间',
    dataIndex: 'updateTime',
    key: 'updateTime',
    render: (text, record) => {
      return text ? (
        <span>{new Date(text * 1000).toLocaleString()}</span>
      ) : (
        <span>加载中...</span>
      );
    },
  },
  {
    title: '大小',
    dataIndex: 'size',
    key: 'size',
    render: (text, record) => {
      if (text === undefined) return <span>加载中...</span>;
      if (text === null) return <span>未知</span>;
      return <span>{text}</span>;
    },
  },
  {
    title: '所有者',
    dataIndex: 'owner',
    key: 'owner',
    render: (text, record) => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
  {
    title: '组',
    dataIndex: 'group',
    key: 'group',
    render: (text, record) => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
  {
    title: '权限',
    dataIndex: 'mode',
    key: 'mode',
    render: (text, record) => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
];

const TableView: React.FC = () => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
  const currentPath = useSelector((state: RootState) => state.file.currentPath);

  const fetchFileList = (path: string): Promise<File[]> =>
    api
      .request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path : path === '' ? '/' : path
      })
      .then((res: GetFileListResponse) => {
        setData(
          res.files.map(file => {
            return {
              name: file.name,
              type: file.type,
            };
          }),
        );
        return res.files;
      });
  const fetchData = (path: string) => {
    fetchFileList(path).then((files: File[]) => {
      api
        .request<GetAllFileDetailsRequest, GetAllFileDetailsResponse>(ApiEnum.GetAllFileDetails, {
          path: path === '' ? '/' : path,
        })
        .then((res: GetAllFileDetailsResponse) => {
          setData(
            res.files
          );
        });
    });
  };

  useEffect(() => {
    fetchData(currentPath);
  }, [currentPath]);

  return (
    <Table<DataType>
      columns={columns}
      dataSource={data}
      pagination={false}
      className={`overflow-auto ${styles['fade-in-down']}`}
      size='small'
      rowKey={'name'}
    />
  );
};

export default TableView;
