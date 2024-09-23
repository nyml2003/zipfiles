import React, { useState, useEffect, useCallback } from 'react';
import { Tree } from 'antd';
import useApi from '@/hooks/useApi';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { ApiEnum } from '@/apis';
import { DownOutlined } from '@ant-design/icons';
const { DirectoryTree } = Tree;

interface DataNode {
  title: string;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
}

const ExplorerPage: React.FC = () => {
  const api = useApi();
  const [treeData, setTreeData] = useState<DataNode[]>([
    { title: '/app', key: '/app', isLeaf: false },
  ]);
  const [expandedKeys, setExpandedKeys] = useState<React.Key[]>([]);

  useEffect(() => {
    handleGetFileList('/app');
  }, []);
  const handleGetFileList = async (path: string) => {
    try {
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path,
      });
      const newTreeData = res.files.map(item => {
        return {
          title: item.name,
          key: `${path}/${item.name}`,
          isLeaf: item.type === 'file',
        };
      });
      setTreeData(updateTreeData(treeData, path, newTreeData));
    } catch (err) {
      console.error(err);
    }
  };

  const updateTreeData = (
    treeData: DataNode[],
    path: string,
    newTreeData: DataNode[],
  ): DataNode[] => {
    return treeData.map((item: DataNode) => {
      if (item.key === path) {
        return {
          ...item,
          children: newTreeData,
        };
      }
      if (item.children) {
        return {
          ...item,
          children: updateTreeData(item.children, path, newTreeData),
        };
      }
      return item;
    });
  };

  const onLoadData = async ({ key, children }: { key: React.Key; children?: DataNode[] }) => {
    const targetNode = treeData.find(item => item.key === key);
    if (targetNode && targetNode.children) {
      return;
    }
    await handleGetFileList(key as string);
  };
  const onExpand = (expandedKeys: React.Key[]) => {
    setExpandedKeys(expandedKeys);
  };

  return (
    <div className='h-full flex flex-col'>
      <div className='p-4 border-b border-gray-200'>Explorer</div>
      <div className='flex-1 overflow-y-auto'>
        <DirectoryTree
          showLine
          multiple
          switcherIcon={<DownOutlined />}
          loadData={onLoadData}
          treeData={treeData}
          expandedKeys={expandedKeys}
          onExpand={onExpand}
        />
      </div>
    </div>
  );
};

export default ExplorerPage;
