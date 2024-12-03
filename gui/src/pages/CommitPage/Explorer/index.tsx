import React, { useContext, useEffect, useMemo, useState } from "react";
import {
  ArrowLeftOutlined,
  ClearOutlined,
  HomeOutlined,
  Loading3QuartersOutlined,
} from "@ant-design/icons";
import { Breadcrumb, Button, Splitter } from "antd";
import TreeMenu from "./TreeMenu";
import TableView from "./TableView";
import { ApiEnum } from "@/apis";
import useApi from "@useApi";
import { Context } from "./store";
import { GetCommitDetailResponse, GetCommitDetailRequest } from "@/apis/GetCommitDetail";
import { ReportError } from "@/stores/NotificationReducer";
import { useDispatch } from "react-redux";

const Explorer: React.FC = () => {
  const { state, actions } = useContext(Context);
  const api = useApi();
  const dispatch = useDispatch();
  const [fresh, setFresh] = useState<boolean>(false);

  const fetchData = async () => {
    try {
      const res = await api.request<GetCommitDetailRequest, GetCommitDetailResponse>(
        ApiEnum.GetCommitDetail,
        {
          uuid: state.commitId,
        },
      );
      actions.updateFiles(res.files);
    } catch (e: unknown) {
      dispatch(
        ReportError({
          state: "error",
          text: "获取提交详情失败",
          description: (e as Error).message,
        }),
      );
    }
  };

  useEffect(() => {
    fetchData();
  }, [fresh]);

  const breadcrumbItems = useMemo(() => {
    if (state.loading) {
      return [];
    }
    const items = [
      {
        title: <HomeOutlined />,
        onClick: () => actions.updatePath(""),
        className: "px-2 py-1 rounded cursor-pointer hover:bg-gray-200 ",
      },
    ];
    if (state.path === "") {
      return items;
    }
    state.path.split("/").forEach((item, index, arr) => {
      const path = arr.slice(0, index + 1).join("/");
      items.push({
        title: <span>{item}</span>,
        onClick: () => actions.updatePath(path),
        className: "px-2 py-1 rounded cursor-pointer hover:bg-gray-200 ",
      });
    });
    return items;
  }, [state.path, state.loading]);

  return (
    <div className='split-container-col grow-item'>
      <div className='flex rounded-xl items-center justify-between bg-gray-100'>
        <div className='flex p-2 items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() =>
              actions.updatePath(state.path.split("/").slice(0, -1).join("/"))
            }></Button>
          <Button
            type='text'
            onClick={() => {
              setFresh((prev) => !prev);
            }}
            icon={<Loading3QuartersOutlined />}></Button>
          <Button
            type='text'
            onClick={() => actions.updatePath("")}
            icon={<ClearOutlined />}></Button>
          <Breadcrumb items={breadcrumbItems} />
        </div>
      </div>
      <div
        className='
        bg-white
        rounded-xl
        m-2
        p-2
        fade-in-down
        grow-item 
        split-container-row
        '>
        {
          <Splitter className='fade-in-down split-container-row grow-item'>
            <Splitter.Panel
              defaultSize='20%'
              min='10%'
              max='70%'
              className='split-container-row grow-item'>
              <TreeMenu />
            </Splitter.Panel>
            <Splitter.Panel className='split-container-row grow-item'>
              <TableView />
            </Splitter.Panel>
          </Splitter>
        }
      </div>
    </div>
  );
};

export default Explorer;
