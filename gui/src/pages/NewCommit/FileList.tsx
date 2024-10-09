import { FileDetail } from '@/apis/GetAllFileDetails';
import Explorer from '@/components/Explorer';
import { RootState } from '@/stores/store';
import { Button, Modal, Table } from 'antd';
import React from 'react';
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
const FileList: React.FC = () => {
  const selectedFile = useSelector((state: RootState) => state.file.selectedFile);

  const handleAdd = () => {
    //打开一个新窗口
    
  };

  const handleDelete = () => {
    console.log('删除');
  };

  return (
    <div className='h-full flex flex-col'>
      <Table<DataType>
        columns={columns}
        dataSource={selectedFile}
        size='small'
        rowKey={'name'}
        className='flex-1 w-full'
      />
      <div className='flex flex-row justify-between m-4'>
        总大小：{selectedFile.reduce((acc, cur) => acc + cur.size, 0)}
        <div className='flex flex-row space-x-2'>
          <Button type='primary' onClick={handleAdd}>
            添加
          </Button>
          <Button type='primary' onClick={handleDelete}>
            删除
          </Button>
        </div>
      </div>
    </div>
  );
};

export default FileList;
