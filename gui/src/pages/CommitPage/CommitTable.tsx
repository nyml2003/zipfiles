import React, { useEffect, useState } from 'react';
import { Button, Table } from 'antd';
import type { TableColumnsType } from 'antd';
import { ApiEnum } from '@/apis';
import useApi from '@/hooks/useApi';
import { CommitLog, GetCommitListRequest, GetCommitListResponse } from '@/apis/GetCommitList';

type DataType = Partial<CommitLog>;

interface CommitTableProps {
  openExplorer: () => void;
}

const CommitTable = ({ openExplorer }: CommitTableProps) => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
  const columns: TableColumnsType<DataType> = [
    {
      title: 'ID',
      dataIndex: 'uuid',
      key: 'uuid',
      ellipsis: true,
    },
    {
      title: '提交者',
      dataIndex: 'author',
      key: 'author',
      ellipsis: true,
    },
    {
      title: '提交时间',
      dataIndex: 'createTime',
      key: 'createTime',
      ellipsis: true,
    },
    {
      title: '提交信息',
      dataIndex: 'message',
      key: 'message',
      ellipsis: true,
    },
    {
      title: '默认路径',
      dataIndex: 'defaultPath',
      key: 'defaultPath',
      ellipsis: true,
    },
    {
      title: '存储路径',
      dataIndex: 'storagePath',
      key: 'storagePath',
      ellipsis: true,
    },
    {
      title: '是否加密',
      dataIndex: 'isEncrypt',
      key: 'isEncrypt',
      render: (value: boolean) => (value ? '是' : '否'),
      ellipsis: true,
    },
    {
      title: '是否删除',
      dataIndex: 'isDelete',
      key: 'isDelete',
      render: (value: boolean) => (value ? '是' : '否'),
      ellipsis: true,
    },
    {
      title: '操作',
      dataIndex: 'operation',
      key: 'operation',
      render: () => <Button onClick={() => openExplorer()}>浏览</Button>,
      ellipsis: true,
    },
  ];
  const fetchData = () => {
    api
      .request<GetCommitListRequest, GetCommitListResponse>(ApiEnum.GetCommitList, {})
      .then(res => {
        console.log(res);
        setData(res.data);
      });
  };

  useEffect(() => {
    fetchData();
  }, []);

  return (
    <Table<DataType>
      columns={columns}
      dataSource={data}
      pagination={false}
      className='overflow-auto fade-in-down rounded-lg'
      size='small'
      rowKey={'name'}
    />
  );
};

export default CommitTable;
