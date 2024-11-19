import React, { useEffect, useRef, useState, useContext } from "react";
import { Table } from "antd";
import type { TableColumnsType } from "antd";
import { FileType, FileTypeToString } from "@/types";
import { FileFilled, FolderFilled } from "@ant-design/icons";
import { Context } from "./store";
import { findFile } from "@/utils";
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
type DataType = Partial<FileDetail>;
const columns: TableColumnsType<DataType> = [
  {
    title: "文件名",
    dataIndex: "name",
    key: "name",
    render: text => {
      return <a>{text}</a>;
    },
    ellipsis: true,
  },
  {
    title: "类型",
    dataIndex: "type",
    ellipsis: true,
    key: "type",
    render: text => {
      if (text === FileType.Directory) {
        return <FolderFilled />;
      }
      if (text === FileType.Regular) {
        return <FileFilled />;
      }
      return FileTypeToString(text);
    },
  },
  {
    title: "创建时间",
    dataIndex: "createTime",
    key: "createTime",
    ellipsis: true,
    render: text => {
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span></span>;
    },
  },
  {
    title: "更新时间",
    dataIndex: "updateTime",
    key: "updateTime",
    ellipsis: true,
    render: text => {
      return text ? <span>{new Date(text * 1000).toLocaleString()}</span> : <span></span>;
    },
  },
  {
    title: "大小",
    dataIndex: "size",
    key: "size",
    ellipsis: true,
    render: text => {
      if (text === undefined) return <span></span>;
      if (text === null) return <span>未知</span>;
      return <span>{text}</span>;
    },
  },
  {
    title: "所有者",
    dataIndex: "owner",
    key: "owner",
    ellipsis: true,
    render: text => {
      return text ? <span>{text}</span> : <span></span>;
    },
  },
  {
    title: "组",
    dataIndex: "group",
    key: "group",
    ellipsis: true,
    render: text => {
      return text ? <span>{text}</span> : <span></span>;
    },
  },
  {
    title: "权限",
    dataIndex: "mode",
    key: "mode",
    ellipsis: true,
    render: text => {
      return text ? <span>{text}</span> : <span></span>;
    },
  },
];
const TableView: React.FC = () => {
  const { state } = useContext(Context);
  const [currentFiles, setCurrentFiles] = useState<DataType[]>([]);
  const tableRef = useRef(null);

  const scrollToRow = (filename: string) => {
    // 确保表格已经被渲染          
    if (tableRef && tableRef.current) {
      const rows = (tableRef.current as HTMLDivElement).ownerDocument.querySelectorAll(
        ".ant-table-row",
      );
      rows.forEach(row => {
        const absoluteRowPath = `${state.path}/${row.getAttribute("data-row-key")}`;
        if (filename.startsWith(absoluteRowPath)) {
          // 滚动到对应行
          row.scrollIntoView({ behavior: "smooth", block: "center" });
        }
      });
    }
  };

  useEffect(() => {
    if (state.file === "") return;
    scrollToRow(state.file);
  }, [state.file]);

  useEffect(() => {
    setCurrentFiles(findFile(state.files, state.path));
  }, [state.files, state.path]);

  return (
    <Table<DataType>
      ref={tableRef}
      columns={columns}
      dataSource={currentFiles}
      pagination={false}
      className='overflow-auto fade-in-down'
      size='small'
      rowKey={"name"}
      rowClassName={record => {
        const absoluteRowPath = `${state.path}/${record.name}`;
        return state.file.startsWith(absoluteRowPath) ? "bg-gray-200" : "";
      }}
    />
  );
};

export default TableView;
