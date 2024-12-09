import React, { useMemo } from "react";
import {
  ArrowLeftOutlined,
  FilterFilled,
  FilterOutlined,
  HomeOutlined,
  Loading3QuartersOutlined,
  RollbackOutlined
} from "@ant-design/icons";
import { Breadcrumb, Button, Space, Splitter, Tooltip } from "antd";
import MyButton from "@/components/Button";
import { handleRefresh, updateCurrentPath, updateIsFiltering } from "@/stores/CreateCommitReducer";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "@/stores/store";
import { BreadcrumbItemType } from "antd/es/breadcrumb/Breadcrumb";
import TreeMenu from "./TreeMenu";
import TableView from "./TableView";
import FilterForm from "./FilterForm";

interface ExplorerProps {
  closeExplorer: () => void;
}

const Explorer: React.FC<ExplorerProps> = ({ closeExplorer }) => {
  const isFiltering = useSelector((state: RootState) => state.createCommit.isFiltering);
  const currentPath = useSelector((state: RootState) => state.createCommit.currentPath);
  const dispatch = useDispatch();

  const breadcrumbItems = useMemo(() => {
    let items: BreadcrumbItemType[] = [
      {
        title: <HomeOutlined />,
        onClick: () => dispatch(updateCurrentPath("")),
        className: "cursor-pointer px-2 py-1 rounded hover:bg-gray-200"
      }
    ];
    if (currentPath === "") {
      return items;
    }
    currentPath
      .split("/")
      .slice(1)
      .forEach((item, index, arr) => {
        const fullPath = "/" + arr.slice(0, index + 1).join("/");
        items.push({
          title: <span>{item}</span>,
          onClick: () => {
            dispatch(updateCurrentPath(fullPath));
          },
          className: "cursor-pointer px-2 py-1 rounded hover:bg-gray-200"
        });
      });
    if (items.length > 4) {
      const first = items[0];
      const last = items[items.length - 1];
      items = [first, { title: <span>...</span>, className: "px-2 py-1 rounded" }, last];
    }
    items[items.length - 1].className = "px-2 py-1 rounded";
    return items;
  }, [currentPath]);

  return (
    <div className='split-container-col grow-item'>
      <div className='flex rounded-xl items-center justify-between bg-gray-100 '>
        <div className='flex p-2 items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() =>
              dispatch(updateCurrentPath(currentPath.split("/").slice(0, -1).join("/")))
            }></Button>
          <Button
            type='text'
            onClick={() => {
              dispatch(handleRefresh());
            }}
            icon={<Loading3QuartersOutlined />}
            disabled={isFiltering}></Button>
          <Breadcrumb items={breadcrumbItems} />
        </div>
        <div className='flex p-2'>
          <Space>
            {!isFiltering && (
              <MyButton variant='success' onClick={closeExplorer}>
                <Tooltip title='返回创建备份界面'>
                  <RollbackOutlined /> 确定
                </Tooltip>
              </MyButton>
            )}
            <MyButton variant='confirm' onClick={() => dispatch(updateIsFiltering(!isFiltering))}>
              {isFiltering ? (
                <Tooltip title='根据当前条件进行筛选'>
                  <FilterFilled /> 确定
                </Tooltip>
              ) : (
                <Tooltip title='筛选'>
                  <FilterOutlined /> 筛选
                </Tooltip>
              )}
            </MyButton>
          </Space>
        </div>
      </div>
      <div className='bg-white rounded-xl fade-in-down grow-item  split-container-row'>
        {isFiltering ? (
          <FilterForm />
        ) : (
          <Splitter className='fade-in-down split-container-row grow-item'>
            <Splitter.Panel
              defaultSize='20%'
              min='10%'
              max='70%'
              className='split-container-row grow-item'>
              <TreeMenu />
            </Splitter.Panel>
            <Splitter.Panel className='split-container-row grow-item'>
              <div>
                <TableView />
              </div>
            </Splitter.Panel>
          </Splitter>
        )}
      </div>
    </div>
  );
};

export default Explorer;
