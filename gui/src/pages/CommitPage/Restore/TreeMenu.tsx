import React, { useState, useEffect, Key, useContext } from "react";
import { Tree, TreeProps } from "antd";
import { DownOutlined } from "@ant-design/icons";
import { FileType } from "@/types";
import { Context } from "./store";
import useApi from "@useApi";
import { ApiEnum } from "@/apis";
import { GetFileDetailListRequest, GetFileDetailListResponse } from "@/apis/GetFileDetailList";
import { ReportError } from "@/stores/NotificationReducer";
import { useDispatch } from "react-redux";
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
  const [treeData, setTreeData] = useState<DataNode[]>([]);
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

  const handleGetFileList = async (path: string) => {
    try {
      const res = await api.request<GetFileDetailListRequest, GetFileDetailListResponse>(
        ApiEnum.GetFileDetailList,
        { path, filter: { type: FileType.Directory } },
      );
      const newTreeData = res.files.map(item => {
        return {
          title: item.name,
          key: `${path}/${item.name}`,
        };
      });
      setTreeData(prevTreeData => updateTreeData(prevTreeData, path, newTreeData));
    } catch (e: unknown) {
      if (!(e instanceof AcceptableError)) {
        return;
      }
      dispatch(
        ReportError({
          state: "error",
          text: "获取文件列表失败",
          description: (e as Error).message,
        }),
      );
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
      if (item.children && item.children.length > 0) {
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
      // form.setFieldsValue({ path: selectedKeys[0].toString() });
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
      className='whitespace-nowrap grow-item h-96 bg-gray-100'
    />
  );
};

export default TreeMenu;
