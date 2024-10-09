import React, { useEffect, useState } from 'react';
import { message, Table } from 'antd';
import type { TableColumnsType } from 'antd';
import { FileDetail } from '@/apis/GetFileDetail';
import { ApiEnum } from '@/apis';
import { FileType, FileTypeToString } from '@/types';
import { CopyOutlined, FileFilled, FolderFilled } from '@ant-design/icons';
import { GetAllFileDetailsRequest, GetAllFileDetailsResponse } from '@/apis/GetAllFileDetails';
import styles from './index.module.less';
import CopyToClipboard from 'react-copy-to-clipboard';
import useApi from '@/hooks/useApi';
import { useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { GetFileListRequest } from '@/apis/GetFileList';
import { cleanObject, filterBy } from '@/utils';

interface DataType extends Partial<FileDetail> {}
const Copy: React.FC<{ text: string; onCopy: () => void; isShow: boolean }> = ({
  text,
  onCopy,
  isShow,
}) => {
  return isShow ? (
    <CopyToClipboard text={text} onCopy={onCopy}>
      <CopyOutlined
        onClick={() => {
          navigator.clipboard.writeText(text);
        }}
      />
    </CopyToClipboard>
  ) : null;
};

const columns: TableColumnsType<DataType> = [
  {
    title: '文件名',
    dataIndex: 'name',

    key: 'name',
    render: (text, record) => {
      return <a>{text}</a>;
    },
    ellipsis: true,
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
    ellipsis: true,
  },
  {
    title: '创建时间',
    dataIndex: 'createTime',
    key: 'createTime',

    render: (text, record) => {
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span>加载中...</span>;
    },
    ellipsis: true,
  },
  {
    title: '更新时间',
    dataIndex: 'updateTime',
    key: 'updateTime',

    render: (text, record) => {
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span>加载中...</span>;
    },
    ellipsis: true,
  },
  {
    title: '大小',
    dataIndex: 'size',
    key: 'size',

    render: (text, record) => {
      if (text === undefined) return <span>加载中...</span>;
      if (text === null) return <span>未知</span>;
      return (
        <span>
          {text}
          <Copy text={text.toString()} onCopy={() => message.success('复制成功')} isShow={text} />
        </span>
      );
    },
    ellipsis: true,
  },
  {
    title: '所有者',
    dataIndex: 'owner',
    key: 'owner',

    render: (text, record) => {
      return text ? (
        <span>
          {text}
          <Copy text={text} onCopy={() => message.success('复制成功')} isShow={true} />
        </span>
      ) : (
        <span>加载中...</span>
      );
    },
    ellipsis: true,
  },
  {
    title: '组',
    dataIndex: 'group',
    key: 'group',

    render: (text, record) => {
      return text ? (
        <span>
          {text}
          <Copy text={text} onCopy={() => message.success('复制成功')} isShow={true} />
        </span>
      ) : (
        <span>加载中...</span>
      );
    },
    ellipsis: true,
  },
  {
    title: '权限',
    dataIndex: 'mode',
    key: 'mode',
    ellipsis: true,
    render: (text, record) => {
      return text ? <span>{text}</span> : <span>加载中...</span>;
    },
  },
];

const TableView: React.FC = () => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
  const currentPath = useSelector((state: RootState) => state.file.currentPath);
  const filter = useSelector((state: RootState) => state.file.filter);
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
