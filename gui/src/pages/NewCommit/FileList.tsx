import { ApiEnum } from '@/apis';
import {
  FileDetail,
  GetAllFileDetailsRequest,
  GetAllFileDetailsResponse,
} from '@/apis/GetAllFileDetails';
import useApi from '@/hooks/useApi';
import { resetSelectedFile, updateExpandedSelectedFile } from '@/stores/CreateCommitReducer';
import { RootState } from '@/stores/store';
import { filterBy } from '@/utils';
import { Button, Table } from 'antd';
import React, { useEffect } from 'react';
import { useDispatch, useSelector } from 'react-redux';

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
  const selectedFile = useSelector((state: RootState) => state.createCommit.selectedFile);
  const filter = useSelector((state: RootState) => state.createCommit.filter);
  const [data, setData] = React.useState<DataType[]>([]);
  const api = useApi();
  const dispatch = useDispatch();

  useEffect(() => {
    setData([]);
    selectedFile.map(async path => {
      const res = await fetchData(path);
      setData(prev => [...prev, ...res]);
    });
  }, [selectedFile]);

  useEffect(() => {
    dispatch(updateExpandedSelectedFile(data.map(file => file.path)));
  }, [data]);

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

  const handleClear = () => {
    dispatch(resetSelectedFile());
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
          <Button type='primary' onClick={handleClear}>
            清空
          </Button>
        </div>
      </div>
    </>
  );
};

export default FileList;
