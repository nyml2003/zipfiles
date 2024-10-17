import React, { useState, useEffect, Key } from 'react';
import { Tree, TreeProps } from 'antd';
import { DownOutlined } from '@ant-design/icons';
import { FileType, LoadingState, NestedFileDetail } from '@/types';
import LoadingWrapper from '@/components/LoadingWrapper';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
const { DirectoryTree } = Tree;
import { setCurrentFile, setCurrentPath } from '@/stores/CommitListReducer';

interface DataNode {
  title: React.ReactNode;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
  expanded?: boolean;
}
function calPath(currentPath: string, file: NestedFileDetail) {
  if (currentPath === '/' && file.name === '/') return '/';
  if (currentPath === '/') {
    return `/${file.name}`;
  }
  return `${currentPath}/${file.name}`;
}
// 递归查找函数
export function findFilesByPath(
  files: NestedFileDetail[],
  targetPath: string,
  currentPath: string,
): NestedFileDetail[] | null {
  for (const file of files) {
    const newCurrentPath = calPath(currentPath, file);
    if (newCurrentPath === targetPath) {
      if (file.type !== FileType.Directory) {
        return [file];
      }
      return file.children;
    }
    if (file.type === FileType.Directory) {
      const result = findFilesByPath(file.children || [], targetPath, newCurrentPath);
      if (result) {
        return result;
      }
    }
  }
  return null;
}

function findFile(
  files: NestedFileDetail[],
  targetPath: string,
  currentPath: string,
): NestedFileDetail | null {
  for (const file of files) {
    const newCurrentPath = calPath(currentPath, file);
    if (newCurrentPath === targetPath) {
      return file;
    }
    if (file.type === FileType.Directory) {
      const result = findFile(file.children || [], targetPath, newCurrentPath);
      if (result) {
        return result;
      }
    }
  }
  return null;
}
const TreeMenu = () => {
  const [treeData, setTreeData] = useState<DataNode[]>([]);
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const [lastClickTime, setLastClickTime] = useState<number>(0);
  const currentPath = useSelector((state: RootState) => state.commitList.currentPath);
  const files = useSelector((state: RootState) => state.commitList.files);
  const dispatch = useDispatch();
  useEffect(() => {
    // 清空已有数据
    setTreeData([]);
    // 清空展开的节点
    setExpandedKeys([]);
    // 加载新的数据
    handleGetFileList(currentPath);
  }, [currentPath]);

  const getRootFile = async (path: string) => {
    const res = findFile(files || [], path === '' ? '/' : path, '/');
    setTreeData([
      {
        title: '.',
        key: path,
        isLeaf: res?.type !== FileType.Directory,
        expanded: true,
      },
    ]);
    setExpandedKeys([path]);
  };

  const handleGetFileList = async (path: string, needLoading = true) => {
    if (needLoading) setLoading(LoadingState.Loading);
    if (path === currentPath) {
      await getRootFile(currentPath);
    }
    try {
      const res = findFilesByPath(files || [], path === '' ? '/' : path, '/');
      if (!res) {
        throw new Error('未找到文件');
      }
      const newTreeData = res.map(item => {
        const isDirectory = item.type === FileType.Directory;
        // title是path减去currentPath + item.name
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
        dispatch(setCurrentPath(selectedKeys[0].toString()));
      }
    } else {
      dispatch(setCurrentFile(selectedKeys[0].toString()));
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
