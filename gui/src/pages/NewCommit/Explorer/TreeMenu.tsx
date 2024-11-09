import React, { useState, useEffect, Key } from 'react';
import { Tree, TreeProps } from 'antd';
import useApi from '@useApi';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { ApiEnum } from '@/apis';
import { DownOutlined } from '@ant-design/icons';
import { FileType, LoadingState } from '@/types';
import LoadingWrapper from '@/components/LoadingWrapper';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import {
  updateCurrentFile,
  updateCurrentPath,
} from '@/stores/CreateCommitReducer';
const { DirectoryTree } = Tree;

interface DataNode {
  title: React.ReactNode;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
  expanded?: boolean;
}

const TreeMenu = () => {
  const api = useApi();
  const [treeData, setTreeData] = useState<DataNode[]>([]);
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const [lastClickTime, setLastClickTime] = useState<number>(0);
  const currentPath = useSelector((state: RootState) => state.createCommit.currentPath);
  const dispatch = useDispatch();
  useEffect(() => {
    // 清空已有数据
    setTreeData([]);
    // 清空展开的节点
    setExpandedKeys([]);
    // 清空选中的节点
    // setCheckedKeys([]);
    // 加载新的数据
    handleGetFileList(currentPath);
  }, [currentPath]);

  // const getRootFile = async (path: string) => {
  //   setTreeData([
  //     {
  //       title: '.',
  //       key: path,
  //       isLeaf: false,
  //       expanded: true,
  //     },
  //   ]);
  //   setExpandedKeys([path]);
  // };

  const handleGetFileList = async (path: string, needLoading = true) => {
    if (needLoading) setLoading(LoadingState.Loading);
    // if (path === currentPath) {
    //   await getRootFile(currentPath);
    // }
    try {
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path,
      });

      const newTreeData = res.files.map(item => {
        const isDirectory = item.type === FileType.Directory;
        return {
          title: item.name,
          key: `${path}/${item.name}`,
          isLeaf: !isDirectory,
        };
      });
      setTreeData(prevTreeData => updateTreeData(prevTreeData, path, newTreeData));
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

  const onLoadData = async (treeNode: DataNode) => {
    const { key } = treeNode;
    const targetNode = treeData.find(item => item.key === key);
    if (targetNode && targetNode.children) {
      return;
    }
    await handleGetFileList(key as string, false);
  };

  const handleSelect: TreeProps['onSelect'] = (selectedKeys, info) => {
    const currentTime = new Date().getTime();
    if (currentTime - lastClickTime < 300) {
      if (!info.node.isLeaf) {
        dispatch(updateCurrentPath(selectedKeys[0].toString()));
      }
    } else {
      dispatch(updateCurrentFile(selectedKeys[0].toString()));
    }
    setLastClickTime(currentTime);
  };

  return (
    <LoadingWrapper loading={loading} hasData={() => treeData.length > 0}>
      <DirectoryTree
        showLine
        multiple
        switcherIcon={<DownOutlined />}
        loadData={onLoadData}
        treeData={treeData}
        onSelect={handleSelect}
        expandedKeys={expandedKeys}
        onExpand={setExpandedKeys}
        className='whitespace-nowrap bg-white grow-item'
      />
    </LoadingWrapper>
  );
};

export default TreeMenu;
