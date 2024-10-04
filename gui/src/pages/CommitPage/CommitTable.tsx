import React, { useEffect, useState } from 'react';
import { Table } from 'antd';
import type { TableColumnsType } from 'antd';
import { ApiEnum } from '@/apis';
import styles from './index.module.less';
import useApi from '@/hooks/useApi';
import { CommitLog, GetCommitListRequest, GetCommitListResponse } from '@/apis/GetCommitList';

interface DataType extends Partial<CommitLog> {}

const columns: TableColumnsType<DataType> = [
  {
    title: 'ID',
    dataIndex: 'uuid',
    key: 'uuid',
  },
  {
    title: '提交者',
    dataIndex: 'author',
    key: 'author',
  },
  {
    title: '提交时间',
    dataIndex: 'createTime',
    key: 'createTime',
  },
  {
    title: '提交信息',
    dataIndex: 'message',
    key: 'message',
  },
  {
    title: '默认路径',
    dataIndex: 'defaultPath',
    key: 'defaultPath',
  },
  {
    title: '存储路径',
    dataIndex: 'storagePath',
    key: 'storagePath',
  },
  {
    title: '是否加密',
    dataIndex: 'isEncrypt',
    key: 'isEncrypt',
    render: (value: boolean) => (value ? '是' : '否'),
  },
  {
    title: '是否删除',
    dataIndex: 'isDelete',
    key: 'isDelete',
    render: (value: boolean) => (value ? '是' : '否'),
  },
];

const CommitTable: React.FC = () => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
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
      className={`overflow-auto ${styles['fade-in-down']} rounded-lg`}
      size='small'
      rowKey={'name'}
    />
  );
};

export default CommitTable;
