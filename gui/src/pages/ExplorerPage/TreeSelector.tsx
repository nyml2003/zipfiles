import React, { useState, useEffect, useCallback, Key } from 'react';
import { Tree, TreeProps } from 'antd';
import useApi from '@/hooks/useApi';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { ApiEnum } from '@/apis';
import { DownOutlined } from '@ant-design/icons';
import LoadingSpinner from '@/components/LoadingSpinner';
const { DirectoryTree } = Tree;

interface DataNode {
  title: string;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
}

interface Props {
  onSelect: (path: string[]) => void;
  currentPath: string;
  setCurrentPath: (path: string) => void;
  setCurrentFile: (file: string) => void;
}

const TreeSelector: React.FC<Props> = ({ onSelect, currentPath, setCurrentPath, setCurrentFile }) => {
  const api = useApi();
  const [treeData, setTreeData] = useState<DataNode[]>([]);
  const [checkedKeys, setCheckedKeys] = useState<Key[]>([]);
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);

  useEffect(() => {
    // 清空已有数据
    setTreeData([]);
    // 清空展开的节点
    setExpandedKeys([]);
    // 清空选中的节点
    setCheckedKeys([]);
    // 加载新的数据
    handleGetFileList(currentPath);
  }, [currentPath]);

  const handleGetFileList = async (path: string) => {
    try {
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path,
      });
      const newTreeData = res.files.map(item => {
        return {
          title: item.name,
          key: `${path}/${item.name}`,
          isLeaf: !(item.type === 'directory'),
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
      console.error(err);
    }
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
    console.log('onLoadData', treeNode);
    const { key, children } = treeNode;
    const targetNode = treeData.find(item => item.key === key);
    if (targetNode && targetNode.children) {
      return;
    }
    await handleGetFileList(key as string);
  };

  const handleCheck: TreeProps['onCheck'] = (checkedKeysValue, info) => {
    setCheckedKeys(checkedKeysValue as Key[]);
    onSelect(info.checkedNodes.map(node => node.key.toString()));
  };

  const handleSelect: TreeProps['onSelect'] = (selectedKeys, info) => {
    if (!info.node.isLeaf) {
      setCurrentPath(selectedKeys[0] as string);
    }
    setCurrentFile(selectedKeys[0] as string);
  };

  return treeData.length === 0 ? (
    <LoadingSpinner />
  ) : (
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
      className='whitespace-nowrap bg-gray-100 flex-1'
    />
  );
};

export default TreeSelector;
