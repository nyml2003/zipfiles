import React, { useState, useEffect, useCallback, Key } from 'react';
import { Button, Tree, TreeProps } from 'antd';
import useApi from '@/hooks/useApi';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { ApiEnum } from '@/apis';
import { ArrowsAltOutlined, DownOutlined } from '@ant-design/icons';
import LoadingSpinner from '@/components/LoadingSpinner';
import { FileType, FileTypeToString, LoadingState } from '@/types';
import NoMoreData from '@/components/NoMoreData';
import LoadingWrapper from '@/components/LoadingWrapper';
const { DirectoryTree } = Tree;

interface DataNode {
  title: React.ReactNode;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
}

interface Props {
  onSelect: (path: string[]) => void;
  currentPath: string;
  setCurrentPath: (path: string) => void;
  setCurrentFile: (file: string) => void;
  refresh: boolean;
  setRefresh: (refresh: boolean) => void;
}

const TreeMenu: React.FC<Props> = ({
  onSelect,
  currentPath,
  setCurrentPath,
  refresh,
  setRefresh,
  setCurrentFile,
}) => {
  const api = useApi();
  const [treeData, setTreeData] = useState<DataNode[]>([]);
  const [checkedKeys, setCheckedKeys] = useState<Key[]>([]);
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const [lastClickTime, setLastClickTime] = useState<number>(0);
  useEffect(() => {
    // 清空已有数据
    setTreeData([]);
    // 清空展开的节点
    setExpandedKeys([]);
    // 清空选中的节点
    setCheckedKeys([]);
    // 加载新的数据
    handleGetFileList(currentPath);
    setCurrentFile(currentPath);
  }, [currentPath]);

  useEffect(() => {
    if (refresh) {
      setRefresh(false);
      handleGetFileList(currentPath);
    }
  }, [refresh]);

  const handleGetFileList = async (path: string, needLoading: boolean = true) => {
    if (needLoading) setLoading(LoadingState.Loading);
    try {
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path: path === '' ? '/' : path,
      });
      //console.log('GetFileListResponse: ', JSON.stringify(res));
      const newTreeData = res.files.map(item => {
        const isDirectory = item.type === FileType.Directory;
        return {
          title: item.name,
          key: `${path}/${item.name}`,
          isLeaf: !isDirectory,
        };
      });
      setTreeData(prevTreeData => updateTreeData(prevTreeData, path, newTreeData));
      // 如果当前选中的文件夹是checkedKeys中的一部分，就把新的文件夹也加入到checkedKeys中
      if (checkedKeys.includes(path)) {
        onSelect([
          ...checkedKeys.map(key => key.toString()),
          ...newTreeData.map(item => item.key.toString()),
        ]);
      }
    } catch (err) {
      console.log('获取文件列表失败: ', err);
    }
    setLoading(LoadingState.Done);
  };

  const updateTreeData = (
    treeData: DataNode[],
    path: string,
    newTreeData: DataNode[],
  ): DataNode[] => {
    if (treeData.length === 0) {
      return newTreeData;
    }
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

  const onLoadData = async (treeNode: any) => {
    const { key, children } = treeNode;
    const targetNode = treeData.find(item => item.key === key);
    if (targetNode && targetNode.children) {
      return;
    }
    await handleGetFileList(key as string, false);
  };

  const handleCheck: TreeProps['onCheck'] = (checkedKeysValue, info) => {
    setCheckedKeys(checkedKeysValue as Key[]);
    onSelect(info.checkedNodes.map(node => node.key.toString()));
  };

  const handleSelect: TreeProps['onSelect'] = (selectedKeys, info) => {
    const currentTime = new Date().getTime();
    if (currentTime - lastClickTime < 300) {
      if (!info.node.isLeaf) {
        setCurrentPath(selectedKeys[0].toString());
      }
    } else {
      setCurrentFile(selectedKeys[0].toString());
    }
    setLastClickTime(currentTime);
  };

  return (
    <LoadingWrapper
      loading={loading}
      hasData={() => treeData.length > 0}
      children={
        <DirectoryTree
          showLine
          checkable
          multiple
          switcherIcon={<DownOutlined />}
          loadData={onLoadData}
          treeData={treeData}
          checkedKeys={checkedKeys}
          onCheck={handleCheck}
          onSelect={handleSelect}
          expandedKeys={expandedKeys}
          onExpand={setExpandedKeys}
          className='whitespace-nowrap bg-gray-100 flex-1 select-none'
        />
      }
    />
  );
};

export default TreeMenu;
