import React, { useState, useEffect, Key } from "react";
import { Spin, Tree, TreeProps } from "antd";
import useApi from "@useApi";
import { GetFileListRequest, GetFileListResponse } from "@/apis/GetFileList";
import { ApiEnum } from "@/apis";
import { DownOutlined } from "@ant-design/icons";
import { FileType, LoadingState } from "@/types";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "@/stores/store";
import { updateCurrentFile, updateCurrentPath } from "@/stores/CreateCommitReducer";
import { ReportError } from "@/stores/NotificationReducer";
import { AcceptableError } from "@/hooks/useApi/types";
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
    handleGetFileList(currentPath);
    return () => {
      setTreeData([]);
      setExpandedKeys([]);
    };
  }, [currentPath]);

  const handleGetFileList = async (path: string, needLoading?: boolean) => {
    if (needLoading) setLoading(LoadingState.Loading);
    try {
      const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
        path
      });
      const newTreeData = res.files.map(item => {
        const isDirectory = item.type === FileType.Directory;
        return {
          title: item.name,
          key: `${path}/${item.name}`,
          isLeaf: !isDirectory
        };
      });
      if (newTreeData.length !== 0) {
        setTreeData(prevTreeData => {
          if (needLoading || !prevTreeData || prevTreeData.length === 0) {
            return newTreeData;
          }
          return updateTreeData(prevTreeData, path, newTreeData);
        });
      }
    } catch (err) {
      if (!(err instanceof AcceptableError)) {
        return;
      }
      dispatch(
        ReportError({
          state: "error",
          text: "获取文件列表失败",
          description: (err as Error).message
        })
      );
    }
    setLoading(LoadingState.Done);
  };

  const updateTreeData = (
    treeData: DataNode[],
    path: string,
    newTreeData: DataNode[]
  ): DataNode[] => {
    return treeData.map((item: DataNode) => {
      if (item.key === path) {
        return {
          ...item,
          children: newTreeData
        };
      }
      if (path.startsWith(item.key as string)) {
        return {
          ...item,
          children: updateTreeData(item.children || [], path, newTreeData)
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

  const handleSelect: TreeProps["onSelect"] = (selectedKeys, info) => {
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

  return loading === LoadingState.Done ? (
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
  ) : (
    <Spin />
  );
};

export default TreeMenu;
