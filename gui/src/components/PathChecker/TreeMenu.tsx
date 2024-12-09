import React, { useState, useEffect, Key, useContext } from "react";
import { Tree, TreeProps } from "antd";
import { DownOutlined } from "@ant-design/icons";
import { FileType, LoadingState } from "@/types";
import { Context } from "./store";
import useApi from "@useApi";
import { ApiEnum } from "@/apis";
import { GetFileDetailListRequest, GetFileDetailListResponse } from "@/apis/GetFileDetailList";
import { ReportError } from "@/stores/NotificationReducer";
import { useDispatch } from "react-redux";
import { AcceptableError } from "@/hooks/useApi/types";
import { DataNode } from "./type";
const { DirectoryTree } = Tree;

interface TreeMenuProps {
  treeData: DataNode[] | null;
  setTreeData: React.Dispatch<React.SetStateAction<DataNode[] | null>>;
  setLoading: React.Dispatch<React.SetStateAction<LoadingState>>;
}

const TreeMenu = ({ treeData, setTreeData, setLoading }: TreeMenuProps) => {
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);
  const [lastClickTime, setLastClickTime] = useState<number>(0);
  const { state, actions } = useContext(Context);
  const dispatch = useDispatch();
  const api = useApi();

  useEffect(() => {
    handleGetFileList(state.path);
    return () => {
      setTreeData([]);
      setExpandedKeys([]);
    };
  }, [state.path, state.fresh]);

  const handleGetFileList = async (path: string, newPath = true) => {
    setLoading(LoadingState.Loading);
    try {
      const res = await api.request<GetFileDetailListRequest, GetFileDetailListResponse>(
        ApiEnum.GetFileDetailList,
        { path, filter: { type: FileType.Directory } }
      );
      const newTreeData = res.files.map(item => {
        return {
          title: item.name,
          key: `${path}/${item.name}`
        };
      });
      if (newTreeData.length !== 0) {
        setTreeData(prevTreeData => {
          if (newPath || !prevTreeData || prevTreeData.length === 0) {
            return newTreeData;
          }
          return updateTreeData(prevTreeData, path, newTreeData);
        });
      }
    } catch (e: unknown) {
      if (!(e instanceof AcceptableError)) {
        return;
      }
      dispatch(
        ReportError({
          state: "error",
          text: "获取文件列表失败",
          description: (e as Error).message
        })
      );
    } finally {
      setLoading(LoadingState.Done);
    }
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

  const handleSelect: TreeProps["onSelect"] = (selectedKeys, info) => {
    const currentTime = new Date().getTime();
    if (currentTime - lastClickTime < 300) {
      if (!info.node.isLeaf) {
        actions.updatePath(selectedKeys[0].toString());
      }
    } else {
      actions.updateFile(selectedKeys[0].toString());
    }
    setLastClickTime(currentTime);
  };

  const handleExpand: TreeProps["onExpand"] = (expandedKeys, info) => {
    if (info.node.key.toString().endsWith("???")) {
      return;
    }
    setExpandedKeys(expandedKeys);
    handleGetFileList(info.node.key as string, false);
  };

  return (
    <DirectoryTree
      showLine
      multiple
      switcherIcon={<DownOutlined />}
      treeData={treeData || []}
      onSelect={handleSelect}
      onExpand={handleExpand}
      expandedKeys={expandedKeys}
      className='
      whitespace-nowrap grow-item bg-gray-100
      '
      style={{
        height: "200px"
      }}
    />
  );
};

export default TreeMenu;
