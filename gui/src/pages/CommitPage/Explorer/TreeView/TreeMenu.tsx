import React, { useState, useEffect, Key } from 'react';
import { Tree, TreeProps } from 'antd';
import useApi from '@/hooks/useApi';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { ApiEnum } from '@/apis';
import { DownOutlined } from '@ant-design/icons';
import { FileType, LoadingState } from '@/types';
import LoadingWrapper from '@/components/LoadingWrapper';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { updateCurrentFile, updateCurrentPath, updateSelectedFile } from '@/stores/CreateCommitReducer';
import { cleanObject } from '@/utils';
const { DirectoryTree } = Tree;
import { GetFileDetailRequest, GetFileDetailResponse } from '@/apis/GetFileDetail';

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
  const [checkedKeys, setCheckedKeys] = useState<Key[]>([]);
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const [lastClickTime, setLastClickTime] = useState<number>(0);
  const currentPath = useSelector((state: RootState) => state.createCommit.currentPath);
  const filter = useSelector((state: RootState) => state.createCommit.filter);
  const dispatch = useDispatch();
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

  const getRootFile = async (path: string) => {
    const res = await api.request<GetFileDetailRequest, GetFileDetailResponse>(
      ApiEnum.GetFileDetail,
      {
        path: path === '' ? '/' : path,
      },
    );
    setTreeData([
      {
        title: '.',
        key: path,
        isLeaf: res.type !== FileType.Directory,
        expanded: true,
      },
    ]);
    setExpandedKeys([path]);
  };

  const handleGetFileList = async (path: string, needLoading: boolean = true) => {
    if (needLoading) setLoading(LoadingState.Loading);
    if (path === currentPath) {
      await getRootFile(currentPath);
    }
    try {
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path: path === '' ? '/' : path,
        filter: cleanObject(filter),
      });

      const newTreeData = res.files.map(item => {
        const isDirectory = item.type === FileType.Directory;
        // title是path减去currentPath + item.name
        return {
          title: item.name,
          key: `${path}/${item.name}`,
          isLeaf: !isDirectory,
        };
      });
      setTreeData(prevTreeData => updateTreeData(prevTreeData, path, newTreeData));
      // 如果当前选中的文件夹是checkedKeys中的一部分，就把新的文件夹也加入到checkedKeys中
      if (checkedKeys.includes(path)) {
        dispatch(
          updateSelectedFile([
            ...checkedKeys.map(key => key.toString()),
            ...newTreeData.map(item => item.key.toString()),
          ]),
        );
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

  const onLoadData = async (treeNode: DataNode) => {
    const { key } = treeNode;
    const targetNode = treeData.find(item => item.key === key);
    if (targetNode && targetNode.children) {
      return;
    }
    await handleGetFileList(key as string, false);
  };

  const handleCheck: TreeProps['onCheck'] = (checkedKeysValue, info) => {
    setCheckedKeys(checkedKeysValue as Key[]);
    dispatch(updateSelectedFile((checkedKeysValue as Key[]).map(key => key.toString())));
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
          className='whitespace-nowrap bg-white grow-item'
        />
      }
    />
  );
};

export default TreeMenu;
