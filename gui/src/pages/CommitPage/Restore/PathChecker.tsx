import React, { useContext, useMemo } from "react";
import { Button } from "antd";
import {
  ArrowLeftOutlined,
  ClearOutlined,
  HomeOutlined,
  Loading3QuartersOutlined,
} from "@ant-design/icons";
import { Breadcrumb } from "antd";
import { Context } from "./store/context";
import TreeMenu from "./TreeMenu";
import { FormInstance } from "antd";

const PathChecker = ({ form }: { form: FormInstance }) => {
  const { state, actions } = useContext(Context);
  const { path } = state;

  const breadcrumbItems = useMemo(() => {
    const items = [
      {
        title: <HomeOutlined />,
        onClick: () => actions.updatePath(""),
        className: "px-2 py-1 rounded cursor-pointer hover:bg-gray-200",
      },
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
          actions.updatePath(fullPath);
          form.setFieldsValue({ path: fullPath });
        },
        className: "px-2 py-1 rounded cursor-pointer hover:bg-gray-200",
      });
    });
    return items;
  }, [state.path]);

  return (
    <div className='split-container-col grow-item'>
      <div className='flex rounded-xl items-center justify-between bg-gray-100'>
        <div className='flex p-2 items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() => {
              const newPath = path.split("/").slice(0, -1).join("/");
              actions.updatePath(newPath);
              form.setFieldsValue({ path: newPath });
            }}></Button>
          <Button
            type='text'
            onClick={() => {
              actions.refresh();
            }}
            icon={<Loading3QuartersOutlined />}></Button>
          <Button
            type='text'
            onClick={() => {
              actions.updatePath("");
              form.setFieldsValue({ path: "" });
            }}
            icon={<ClearOutlined />}></Button>
          <Breadcrumb items={breadcrumbItems} />
        </div>
        <div>
          <Button
            type='text'
            onClick={() => {
              console.log(JSON.stringify(state, null, 2));
            }}>
            打印
          </Button>
        </div>
      </div>
      <div className='bg-white rounded-xl m-2 p-2 fade-in-down grow-item split-container-row'>
        <TreeMenu />
      </div>
    </div>
  );
};

export default PathChecker;
