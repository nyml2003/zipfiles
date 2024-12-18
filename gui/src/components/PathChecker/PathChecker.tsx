import React, {
  useContext,
  useMemo,
  useState
} from "react";
import { Button, message } from "antd";
import {
  ArrowLeftOutlined,
  FolderAddOutlined,
  HomeOutlined,
  Loading3QuartersOutlined
} from "@ant-design/icons";
import { Breadcrumb } from "antd";
import { Context } from "./store";
import TreeMenu from "./TreeMenu";
import { DataNode } from "./type";
import { LoadingState } from "@/types";
import useApi from "@useApi";
import { CreateNullFolderRequest, CreateNullFolderResponse } from "@/apis/CreateNullFolder";
import { ApiEnum } from "@/apis";
import { BreadcrumbItemType } from "antd/es/breadcrumb/Breadcrumb";

const PathChecker = () => {
  const { state, actions } = useContext(Context);
  const [treeData, setTreeData] = useState<DataNode[] | null>(null);
  const [loading, setLoading] = useState<LoadingState>(LoadingState.Done);
  const [creatingNewFolder, setCreatingNewFolder] = useState<boolean>(false);
  const api = useApi();

  // 在当前路径下创建一个新的文件夹
  const createNullFolder = () => {
    setTreeData(prev => {
      if (prev === null) {
        return null;
      }
      setCreatingNewFolder(true);
      const newData = [...prev];
      const newFolderItem = {
        title: (
          <input
            ref={(el: HTMLInputElement) => {
              if (el) {
                el.focus();
              }
            }}
            className='text-black bg-white border-none focus:outline-none select-all'
            onKeyDown={e => {
              if (e.key === "Enter") {
                handleFolderNameChange(e.currentTarget.value);
              }
            }}
            onBlur={e => {
              handleFolderNameChange(e.target.value);
            }}
            placeholder='NewFolderName'
          />
        ),
        key: `${state.path}/???`
      };
      newData.push(newFolderItem);
      return newData;
    });
  };

  // 用户确定新文件夹名称后的回调函数
  const handleFolderNameChange = async (newName: string) => {
    if (newName === "") {
      setCreatingNewFolder(false);
      setTreeData(prev => {
        if (!prev) return null;
        return prev.filter(node => node.key !== `${state.path}/???`);
      });
      actions.refresh();
      actions.updateFile("");
      return;
    }
    setTreeData(prev => {
      if (!prev) return null;
      const newTreeData = [...prev];
      const newFolderIndex = newTreeData.findIndex(node => node.key === `${state.path}/???`);
      if (newFolderIndex !== -1) {
        newTreeData[newFolderIndex] = {
          ...newTreeData[newFolderIndex],
          title: newName,
          key: `${state.path}/${newName}`
        };
      }
      setCreatingNewFolder(false);
      return newTreeData;
    });
    try {
      await api.request<CreateNullFolderRequest, CreateNullFolderResponse>(
        ApiEnum.CreateNullFolder,
        {
          path: state.path,
          newFolderName: newName
        }
      );
      actions.updateFile(`${state.path}/${newName}`);
    } catch {
      message.error("创建文件夹失败");
      actions.refresh();
      return;
    }
  };

  // 生成面包屑导航, 当路径过长时, 只显示前后两个
  const breadcrumbItems = useMemo(() => {
    let items: BreadcrumbItemType[] = [
      {
        title: <HomeOutlined />,
        onClick: () => actions.updatePath(""),
        className: "px-2 py-1 rounded cursor-pointer hover:bg-gray-200"
      }
    ];
    if (state.path === "") {
      return items;
    }
    const path = state.path.slice(1);
    path.split("/").forEach((item, index, arr) => {
      const fullPath = "/" + arr.slice(0, index + 1).join("/");
      items.push({
        title: <span>{item}</span>,
        onClick: () => {
          setTreeData(null);
          actions.updatePath(fullPath);
          actions.refresh();
        },
        className: "px-2 py-1 rounded cursor-pointer hover:bg-gray-200"
      });
    });
    if (items.length > 4) {
      const first = items[0];
      const last = items[items.length - 1];
      items = [
        first,
        {
          title: <span>...</span>,
          className: "px-2 py-1 rounded"
        },
        last
      ];
    }
    return items;
  }, [state.path]);

  return (
    <div className='split-container-col grow-item my-2'>
      <div className='flex rounded-xl items-center justify-between bg-gray-100'>
        <div className='flex p-2 items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() => {
              const newPath = state.path.split("/").slice(0, -1).join("/");
              actions.updatePath(newPath);
            }}
            disabled={state.path === "" || loading === LoadingState.Loading}></Button>
          <Button
            type='text'
            onClick={() => {
              actions.refresh();
            }}
            disabled={loading === LoadingState.Loading}
            icon={<Loading3QuartersOutlined />}></Button>
          <Button
            type='text'
            onClick={() => createNullFolder()}
            disabled={loading === LoadingState.Loading || creatingNewFolder}
            icon={<FolderAddOutlined />}></Button>
          <Breadcrumb items={breadcrumbItems} />
        </div>
      </div>
      <div className='bg-white rounded-xl mt-2 fade-in-down grow-item split-container-row'>
        <TreeMenu treeData={treeData} setTreeData={setTreeData} setLoading={setLoading} />
      </div>
    </div>
  );
};

export default PathChecker;
