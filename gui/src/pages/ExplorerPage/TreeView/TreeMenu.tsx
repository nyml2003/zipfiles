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
import { updateCurrentFile, updateCurrentPath, updateSelectedFile } from '@/stores/file/reducer';
import { cleanObject } from '@/utils';
const { DirectoryTree } = Tree;

interface DataNode {
  title: React.ReactNode;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
}

const TreeMenu = () => {
  const api = useApi();
  const [treeData, setTreeData] = useState<DataNode[]>([]);
  const [checkedKeys, setCheckedKeys] = useState<Key[]>([]);
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const [lastClickTime, setLastClickTime] = useState<number>(0);
  const currentPath = useSelector((state: RootState) => state.file.currentPath);
  const filter = useSelector((state: RootState) => state.file.filter);
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

  const handleGetFileList = async (path: string, needLoading: boolean = true) => {
    if (needLoading) setLoading(LoadingState.Loading);
    try {
      console.log('cleanObject(filter): ', JSON.stringify(cleanObject(filter)));
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path: path === '' ? '/' : path,
        filter: cleanObject(filter),
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
          className='whitespace-nowrap bg-gray-100 flex-1'
        />
      }
    />
  );
};

export default TreeMenu;
