import React, { useEffect, useState } from 'react';
import { Divider, Radio, Table } from 'antd';
import type { TableColumnsType, TableProps } from 'antd';
import { FileDetail, GetFileDetailRequest } from '@/apis/GetFileDetail';
import { File, GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import useApi from '@/hooks/useApi';
import { ApiEnum } from '@/apis';
import { FileType, FileTypeToString } from '@/types';
import { FileFilled, FolderFilled } from '@ant-design/icons';
import { GetAllFileDetailsRequest, GetAllFileDetailsResponse } from '@/apis/GetAllFileDetails';

interface DataType extends Partial<FileDetail> {
  hasDetail: boolean;
}

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
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
  {
    title: '更新时间',
    dataIndex: 'updateTime',
    key: 'updateTime',
    render: (text, record) => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
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

interface TableViewProps {
  currentPath: string;
  refresh: boolean;
  setRefresh: (refresh: boolean) => void;
}

const TableView: React.FC<TableViewProps> = ({ currentPath, refresh, setRefresh: setRrefresh }) => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);

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
              hasDetail: false,
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
            res.files.map(file => {
              return {
                ...file,
                hasDetail: true,
              };
            }),
          );
        });
    });
  };

  useEffect(() => {
    fetchData(currentPath);
  }, [currentPath]);

  useEffect(() => {
    if (refresh) {
      setRrefresh(false);
      fetchData(currentPath);
    }
  }, [refresh]);

  return (
    <Table<DataType>
      columns={columns}
      dataSource={data}
      pagination={false}
      className='overflow-auto'
      size='small'
      rowKey={'name'}
    />
  );
};

export default TableView;
