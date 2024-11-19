import React, { useState, useEffect, Key, useContext, useCallback } from "react";
import { Tree, TreeProps } from "antd";
import { DownOutlined } from "@ant-design/icons";
import { FileType } from "@/types";
import { Context } from "./store";
import { findFile } from "@/utils";
const { DirectoryTree } = Tree;
interface FileDetail {
  name: string;
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}
interface DataNode {
  title: React.ReactNode;
  key: string;
  isLeaf?: boolean;
  children?: DataNode[];
  expanded?: boolean;
}

const TreeMenu = () => {
  const [treeData, setTreeData] = useState<DataNode[]>([]);
  const [expandedKeys, setExpandedKeys] = useState<Key[]>([]);
  const [lastClickTime, setLastClickTime] = useState<number>(0);
  const { state, actions } = useContext(Context);

  useEffect(() => {
    handleGetFileList(state.path);
    return () => {
      setTreeData([]);
      setExpandedKeys([]);
    };
  }, [state.path, state.files]);

  const handleGetFileList = useCallback(
  (path: string) => {
      const res = findFile(state.files, path) as FileDetail[];
      const newTreeData = res.map(item => {
        const isDirectory = item.type === FileType.Directory;
        return {
          title: item.name,
          key: path === "" ? item.name : `${path}/${item.name}`,
          isLeaf: !isDirectory,
        };
      });
      setTreeData(prevTreeData => updateTreeData(prevTreeData, path, newTreeData));
    },
    [state.files],
  );

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
    setExpandedKeys(expandedKeys);
    handleGetFileList(info.node.key as string);
  };

  return (
    <DirectoryTree
      showLine
      multiple
      switcherIcon={<DownOutlined />}
      treeData={treeData}
      onSelect={handleSelect}
      onExpand={handleExpand}
      expandedKeys={expandedKeys}
      className='whitespace-nowrap bg-white grow-item'
    />
  );
};

export default TreeMenu;
