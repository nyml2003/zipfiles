import React, { useEffect, useRef, useState } from 'react';
import { Table } from 'antd';
import type { TableColumnsType } from 'antd';
import { FileDetail } from '@/apis/GetFileDetailList';
import { ApiEnum } from '@/apis';
import { FileType, FileTypeToString } from '@/types';
import { FileFilled, FolderFilled } from '@ant-design/icons';
import { GetAllFileDetailsRequest, GetAllFileDetailsResponse } from '@/apis/GetAllFileDetails';
import useApi from '@/hooks/useApi';
import { useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { filterBy } from '@/utils';

type DataType = Partial<FileDetail>;

const columns: TableColumnsType<DataType> = [
  {
    title: '文件名',
    dataIndex: 'name',

    key: 'name',
    render: text => {
      return <a>{text}</a>;
    },
    ellipsis: true,
  },
  {
    title: '类型',
    dataIndex: 'type',
    ellipsis: true,
    key: 'type',
    render: text => {
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
    ellipsis: true,
    render: text => {
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span>加载中...</span>;
    },
  },
  {
    title: '更新时间',
    dataIndex: 'updateTime',
    key: 'updateTime',
    ellipsis: true,
    render: text => {
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span>加载中...</span>;
    },
  },
  {
    title: '大小',
    dataIndex: 'size',
    key: 'size',
    ellipsis: true,
    render: text => {
      if (text === undefined) return <span>加载中...</span>;
      if (text === null) return <span>未知</span>;
      return <span>{text}</span>;
    },
  },
  {
    title: '所有者',
    dataIndex: 'owner',
    key: 'owner',
    ellipsis: true,
    render: text => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
  {
    title: '组',
    dataIndex: 'group',
    key: 'group',
    ellipsis: true,
    render: text => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
  {
    title: '权限',
    dataIndex: 'mode',
    key: 'mode',
    ellipsis: true,
    render: text => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
];

const TableView: React.FC = () => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
  const currentPath = useSelector((state: RootState) => state.createCommit.currentPath);
  const filter = useSelector((state: RootState) => state.createCommit.filter);
  const currentFile = useSelector((state: RootState) => state.createCommit.currentFile);
  const fetchData = (path: string) => {
    api
      .request<GetAllFileDetailsRequest, GetAllFileDetailsResponse>(ApiEnum.GetAllFileDetails, {
        path: path === '' ? '/' : path,
      })
      .then((res: GetAllFileDetailsResponse) => {
        setData(filterBy(res.files, filter));
      });
  };

  useEffect(() => {
    fetchData(currentPath);
  }, [currentPath]);

  const tableRef = useRef(null);

  const scrollToRow = (filename: string) => {
    // 确保表格已经被渲染
    if (tableRef && tableRef.current) {
      const rows = (tableRef.current as HTMLDivElement).ownerDocument.querySelectorAll(
        '.ant-table-row',
      );
      rows.forEach(row => {
        const prefix = currentPath === '/' ? '' : currentPath;
        const absoluteRowPath = `${prefix}/${row.getAttribute('data-row-key')}`;
        if (absoluteRowPath === filename) {
          // 滚动到对应行
          row.scrollIntoView({ behavior: 'smooth', block: 'center' });
        }
      });
    }
  };

  useEffect(() => {
    if (currentFile === '') return;
    scrollToRow(currentFile);
  }, [currentFile]);

  return (
    <Table<DataType>
      ref={tableRef}
      columns={columns}
      dataSource={data}
      pagination={false}
      className='overflow-auto fade-in-down'
      size='small'
      rowKey={'name'}
      rowClassName={record => {
        const prefix = currentPath === '/' ? '' : currentPath;
        const absoluteRowPath = `${prefix}/${record.name}`;
        return absoluteRowPath === currentFile ? 'bg-gray-200' : '';
      }}
    />
  );
};

export default TableView;
