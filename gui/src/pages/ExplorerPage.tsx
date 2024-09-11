import React, { useState } from 'react';
import { Button, Tree } from 'antd';
import useApi from '@/hooks/useApi';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { ApiEnum } from '@/apis';

interface DataNode {
  title: string;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
}


// It's just a simple demo. You can use tree map to optimize update perf.
const updateTreeData = (list: DataNode[], key: React.Key, children: DataNode[]): DataNode[] =>
  list.map(node => {
    if (node.key === key) {
      return {
        ...node,
        children,
      };
    }
    if (node.children) {
      return {
        ...node,
        children: updateTreeData(node.children, key, children),
      };
    }
    return node;
  });

const ExplorerPage: React.FC = () => {
    const api = useApi()
    const [treeData, setTreeData] = useState<DataNode[]>([]);

    const onLoadData = ({ key, children }: any) =>
    {
        return new Promise<void>((resolve) => {
            if (children) {
                resolve();
                return;
            }
            const path = key.split('-').slice(1).join('/')
            handleGetFileList(path, key)
            resolve();
        });
  }

    const handleGetFileList = (path: string = '/app', key: string = 'app') => {
        api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, { path: path }).then((res) => {
            console.log(res)
            setTreeData(res.files.map((item, index) => {
                return {
                    title: item.name,
                    key: `${key}-${item.name}`,
                    isLeaf: item.type === 'file'
                }
            }))
        }).catch((err) => {
            console.error(err)
        })
    }

  return (
    <>
          <Tree loadData={onLoadData} treeData={treeData} />
          <Button onClick={() => handleGetFileList()}>Get File List</Button>
    
      </>
  );
};

export default ExplorerPage;
