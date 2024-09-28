import React, { useEffect, useState } from 'react';
import { Divider, Radio, Table } from 'antd';
import type { TableColumnsType, TableProps } from 'antd';
import { FileDetail, GetFileDetailRequest } from '@/apis/GetFileDetail';
import { File, GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import useApi from '@/hooks/useApi';
import { ApiEnum } from '@/apis';
import { FileType, FileTypeToString } from '@/types';
import { FileFilled, FolderFilled } from '@ant-design/icons';

interface DataType extends Partial<FileDetail> {
  key: string;
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
      return text ? <span>{text}</span> : <span>加载中...</span>;
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
}

const TableView: React.FC<TableViewProps> = ({ currentPath }) => {
  const api = useApi();
  const [files, setFiles] = useState<File[]>([]);
  const [data, setData] = useState<DataType[]>([]);
  useEffect(() => {
    api
      .request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path: currentPath,
      })
      .then((res: GetFileListResponse) => {
        setFiles(res.files);
        setData(
          res.files.map(file => {
            return {
              name: file.name,
              type: file.type,
              key: file.name,
            };
          }),
        );

        const fetchDetail = (file: File): Promise<FileDetail> =>
          api
            .request<GetFileDetailRequest, FileDetail>(ApiEnum.GetFileDetail, {
              path: `${currentPath}/${file.name}`,
            })
            .then(res => {
              return res;
            })
            .catch(e => {
              // 重新请求
              return fetchDetail(file);
            });

        res.files.forEach(file => {
          fetchDetail(file).then(res => {
            setData((prevData: DataType[]) => {
              return prevData.map(data => {
                if (data.name === file.name) {
                  return {...res, key: file.name};
                }
                return data;
              });
            });
          });
        });
      })
      .catch(e => {
        console.log('获取文件列表失败: ', e);
      });
  }, [currentPath]);
  return (
    <div>
      <Table<DataType> columns={columns} dataSource={data} pagination={false} />
    </div>
  );
};

export default TableView;
