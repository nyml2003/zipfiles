import { ApiEnum } from '@/apis';
import {
  FileDetail,
  GetAllFileDetailsRequest,
  GetAllFileDetailsResponse,
} from '@/apis/GetAllFileDetails';
import useApi from '@/hooks/useApi';
import { RootState } from '@/stores/store';
import { filterBy } from '@/utils';
import { Button, Table } from 'antd';
import React, { useEffect } from 'react';
import { useSelector } from 'react-redux';

const columns = [
  {
    title: '文件名',
    dataIndex: 'name',
    key: 'name',
  },
  {
    title: '类型',
    dataIndex: 'type',
    key: 'type',
  },
  {
    title: '创建时间',
    dataIndex: 'createTime',
    key: 'createTime',
  },
  {
    title: '更新时间',
    dataIndex: 'updateTime',
    key: 'updateTime',
  },
  {
    title: '大小',
    dataIndex: 'size',
    key: 'size',
  },
  {
    title: '所有者',
    dataIndex: 'owner',
    key: 'owner',
  },
  {
    title: '组',
    dataIndex: 'group',
    key: 'group',
  },
  {
    title: '权限',
    dataIndex: 'mode',
    key: 'mode',
  },
];

type DataType = FileDetail;

interface FileListProps {
  addExplorer: () => void;
}

const FileList: React.FC<FileListProps> = ({ addExplorer }) => {
  const selectedFile = useSelector((state: RootState) => state.file.selectedFile);
  const filter = useSelector((state: RootState) => state.file.filter);
  const [data, setData] = React.useState<DataType[]>([]);
  const api = useApi();

  useEffect(() => {
    setData([]);
    selectedFile.map(async (path) => {
      const res = await fetchData(path);
      setData((prev) => [...prev, ...res]);
    });
  }, [selectedFile]);

  const handleAdd = () => {
    addExplorer();
  };

  const handleDelete = () => {
    console.log('删除');
  };

  const fetchData = async (path: string) => {
    const res = await api.request<GetAllFileDetailsRequest, GetAllFileDetailsResponse>(
      ApiEnum.GetAllFileDetails,
      {
        path: path === '' ? '/' : path,
      },
    );
    return filterBy(res.files, filter);
  };

  return (
    <>
      <Table<DataType> columns={columns} dataSource={data} size='small' rowKey={'name'} />
      <div className='m-4 flex flex-row justify-between items-center'>
        <div className='text-center'>总大小：{data.reduce((acc, cur) => acc + cur.size, 0)}</div>
        <div className='space-x-2'>
          <Button type='primary' onClick={handleAdd}>
            添加
          </Button>
          <Button type='primary' onClick={handleDelete}>
            删除
          </Button>
        </div>
      </div>
    </>
  );
};

export default FileList;
