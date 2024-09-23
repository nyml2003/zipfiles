import React, { useState, useEffect, useCallback, Key } from 'react';
import { Tree, TreeProps } from 'antd';
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

interface Props {
  onSelect?: (path: string[]) => void;
}

const TreeSelector: React.FC<Props> = (props: Props) => {
  const onSelect = props.onSelect || (() => {});
  const api = useApi();
  const [treeData, setTreeData] = useState<DataNode[]>([
    { title: 'app', key: '/app', isLeaf: false },
  ]);
  const [checkedKeys, setCheckedKeys] = useState<Key[]>([]);

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
    await handleGetFileList(key as string);
  };

  const handleCheck: TreeProps['onCheck'] = (checkedKeysValue, info) => {
    setCheckedKeys(checkedKeysValue as Key[]);
    onSelect(info.checkedNodes.map(node => node.key.toString()));
  };

  return (
    <DirectoryTree
      showLine
      checkable
      multiple
      switcherIcon={<DownOutlined />}
      loadData={onLoadData}
      treeData={treeData}
      checkedKeys={checkedKeys}
      onCheck={handleCheck}
    />
  );
};

export default TreeSelector;
