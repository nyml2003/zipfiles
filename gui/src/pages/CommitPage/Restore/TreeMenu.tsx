import React, { useState, useEffect, Key, useContext } from "react";
import { Tree, TreeProps } from "antd";
import { DownOutlined } from "@ant-design/icons";
import { FileType } from "@/types";
import { Context } from "./store/context";
import useApi from "@useApi";
import { GetFileListRequest, GetFileListResponse } from "@/apis/GetFileList";
import { ApiEnum } from "@/apis";
import { GetFileDetailListRequest, GetFileDetailListResponse } from "@/apis/GetFileDetailList";
const { DirectoryTree } = Tree;
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
  const api = useApi();

  useEffect(() => {
    handleGetFileList(state.path);
    return () => {
      setTreeData([]);
      setExpandedKeys([]);
    };
  }, [state.path, state.fresh]);

  const handleGetFileList = async (path: string) => {
    const res = await api.request<GetFileDetailListRequest, GetFileDetailListResponse>(
      ApiEnum.GetFileDetailList,
      { path, filter: { type: FileType.Directory } },
    );
    const newTreeData = res.files.map(item => {
      const isDirectory = item.type === FileType.Directory;
      return {
        title: item.name,
        key: `${path}/${item.name}`,
      };
    });
    console.log(newTreeData);
    setTreeData(prevTreeData => updateTreeData(prevTreeData, path, newTreeData));
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

  const handleSelect: TreeProps["onSelect"] = (selectedKeys, info) => {
    const currentTime = new Date().getTime();
    if (currentTime - lastClickTime < 300) {
      if (!info.node.isLeaf) {
        actions.updatePath(selectedKeys[0].toString());
      }
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
      className='whitespace-nowrap bg-white grow-item min-h-32'
    />
  );
};

export default TreeMenu;
