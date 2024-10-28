import { ApiEnum } from '@/apis';
import { GetFileDetailRequest, GetFileDetailResponse } from '@/apis/GetFileDetail';
import useApi from '@/hooks/useApi';
import { updateSelectedDirectory, updateSelectedFile } from '@/stores/CreateCommitReducer';
import { RootState } from '@/stores/store';
import { FileType } from '@/types';
import { Button, List, Table } from 'antd';
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
  const directiories = useSelector(
    (state: RootState) => state.createCommit.selectedFile.directories,
  );
  const [fileData, setFileData] = React.useState<FileDetail[]>([]);
  const dispatch = useDispatch();

  const api = useApi();

  useEffect(() => {
    Promise.all(
      files.map(async (file: { name: string; path: string }) => {
        const res = await fetchFileDetail(file.path, file.name);
        return res;
      }),
    ).then(res => {
      setFileData(res);
    });
  }, [files]);

  const handleAdd = () => {
    addExplorer();
  };

  const handleDelete = () => {
    console.log('删除');
  };

  const fetchFileDetail = async (path: string, name: string) => {
    const res = await api.request<GetFileDetailRequest, GetFileDetailResponse>(
      ApiEnum.GetFileDetail,
      {
        path,
        name,
      },
    );
    return res;
  };

  const handleClear = () => {
    dispatch(updateSelectedFile([]));
    dispatch(updateSelectedDirectory([]));
  };

  return (
    <div className='grow-item'>
      <h2 className='text-xl font-bold px-2 py-1 m-2'>文件列表</h2>
      <Table<FileDetail> columns={columns} dataSource={fileData} size='small' rowKey={'name'} />
      <h2 className='text-xl font-bold px-2 py-1 m-2'>路径列表</h2>
      <List<string>
        dataSource={directiories}
        size='small'
        renderItem={item =>
          item === '' ? <List.Item> / </List.Item> : <List.Item> {item} </List.Item>
        }
      />
      <div className='m-4 flex flex-row justify-end items-center'>
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
    </div>
  );
};

export default FileList;
