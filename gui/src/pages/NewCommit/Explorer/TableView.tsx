import React, { useCallback, useEffect, useRef, useState } from "react";
import { Checkbox, Table } from "antd";
import type { TableColumnsType } from "antd";
import { ApiEnum } from "@/apis";
import { FileType, FileTypeToString } from "@/types";
import { GetFileDetailListRequest, GetFileDetailListResponse } from "@/apis/GetFileDetailList";
import useApi from "@useApi";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "@/stores/store";
import {
  addSelectedDirectory,
  addSelectedFile,
  removeSelectedFile,
  removeSelectedDirectory
} from "@/stores/CreateCommitReducer";
import { ReportError } from "@/stores/NotificationReducer";
import { convertBytesToHumanReadable, modeToString } from "@/utils";
import { AcceptableError } from "@/hooks/useApi/types";
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
const TableView: React.FC = () => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
  const currentPath = useSelector((state: RootState) => state.createCommit.currentPath);
  const filter = useSelector((state: RootState) => state.createCommit.filter);
  const currentFile = useSelector((state: RootState) => state.createCommit.currentFile);
  const selectedFiles = useSelector((state: RootState) => state.createCommit.selectedFile.files);
  const selectedDirectories = useSelector(
    (state: RootState) => state.createCommit.selectedFile.directories
  );
  const fresh = useSelector((state: RootState) => state.createCommit.fresh);
  const dispatch = useDispatch();
  const renderFileCheckbox = useCallback(
    (_: string, record: DataType) => {
      if (record.path === undefined) return;
      if (record.name === undefined) return;
      let checked =
        selectedFiles.findIndex(file => file.path === record.path && file.name === record.name) !==
        -1;
      let disabled = false;
      const filePath = record.path + "/" + record.name;
      for (const directory of selectedDirectories) {
        if (filePath.startsWith(directory.name)) {
          disabled = true;
          checked = true;
          break;
        }
      }
      const file = {
        path: record.path,
        name: record.name
      };

      const handleClick = () => {
        if (checked) {
          dispatch(removeSelectedFile(file));
        } else {
          dispatch(addSelectedFile(file));
        }
      };

      return (
        <Checkbox
          disabled={disabled}
          checked={checked}
          onClick={() => {
            handleClick();
          }}></Checkbox>
      );
    },
    [selectedFiles, selectedDirectories]
  );
  const renderDirectoryCheckbox = useCallback(() => {
    let checked = false;
    let disabled = false;
    for (const directory of selectedDirectories) {
      if (currentPath.startsWith(directory.name)) {
        checked = true;
        if (currentPath !== directory.name) disabled = true;
        break;
      }
    }

    const handleClick = () => {
      if (checked) {
        dispatch(removeSelectedDirectory(currentPath));
      } else {
        dispatch(addSelectedDirectory(currentPath));
      }
    };

    return (
      <Checkbox checked={checked} disabled={disabled} onClick={() => handleClick()}></Checkbox>
    );
  }, [selectedDirectories, currentPath]);
  const columns: TableColumnsType<DataType> = [
    {
      title: renderDirectoryCheckbox(),
      dataIndex: "operation",
      key: "operation",
      render: (text, record: DataType) => {
        return renderFileCheckbox(text, record);
      },
      align: "center",
      ellipsis: true,
      width: 50
    },
    {
      title: "文件名",
      dataIndex: "name",
      key: "name",
      render: text => {
        return <a>{text}</a>;
      },
      ellipsis: true,
      width: 100
    },
    {
      title: "类型",
      dataIndex: "type",
      ellipsis: true,
      key: "type",
      render: text => {
        return FileTypeToString(text);
      },
      width: 75
    },
    {
      title: "创建时间",
      dataIndex: "createTime",
      key: "createTime",
      ellipsis: true,
      render: text => {
        return text ? (
          <span>{new Date(text * 1000).toLocaleString()}</span>
        ) : (
          <span>加载中...</span>
        );
      },
      width: 150
    },
    {
      title: "更新时间",
      dataIndex: "updateTime",
      key: "updateTime",
      ellipsis: true,
      render: text => {
        return text ? (
          <span>{new Date(text * 1000).toLocaleString()}</span>
        ) : (
          <span>加载中...</span>
        );
      },
      width: 150
    },
    {
      title: "大小",
      dataIndex: "size",
      key: "size",
      ellipsis: true,
      render: text => {
        if (text === undefined) return <span>加载中...</span>;
        if (text === null) return <span>未知</span>;
        if (text === 0) return <span>-</span>;
        return <span>{convertBytesToHumanReadable(text)}</span>;
      },
      width: 100
    },
    {
      title: "所有者",
      dataIndex: "owner",
      key: "owner",
      ellipsis: true,
      render: text => {
        return text ? <span>{text}</span> : <span>加载中...</span>;
      }
    },
    {
      title: "组",
      dataIndex: "group",
      key: "group",
      ellipsis: true,
      render: text => {
        return text ? <span>{text}</span> : <span>加载中...</span>;
      }
    },
    {
      title: "权限",
      dataIndex: "mode",
      key: "mode",
      ellipsis: true,
      render: text => {
        return text ? <span>{modeToString(text)}</span> : <span>加载中...</span>;
      },
      width: 75,
      align: "center"
    }
  ];

  const fetchData = (path: string) => {
    api
      .request<GetFileDetailListRequest, GetFileDetailListResponse>(ApiEnum.GetFileDetailList, {
        path,
        filter
      })
      .then((res: GetFileDetailListResponse) => {
        setData(res.files);
      })
      .catch((err: Error) => {
        if (!(err instanceof AcceptableError)) {
          return;
        }
        dispatch(
          ReportError({ state: "error", text: "获取文件详情失败", description: err.message })
        );
      });
  };

  useEffect(() => {
    fetchData(currentPath);
  }, [currentPath, filter, fresh]);

  const tableRef = useRef(null);

  const scrollToRow = (filename: string) => {
    // 确保表格已经被渲染
    if (tableRef && tableRef.current) {
      const rows = (tableRef.current as HTMLDivElement).ownerDocument.querySelectorAll(
        ".ant-table-row"
      );
      rows.forEach(row => {
        const prefix = currentPath === "/" ? "" : currentPath;
        const absoluteRowPath = `${prefix}/${row.getAttribute("data-row-key")}`;
        if (filename.startsWith(absoluteRowPath)) {
          // 滚动到对应行
          row.scrollIntoView({ behavior: "smooth", block: "center" });
        }
      });
    }
  };

  useEffect(() => {
    if (currentFile === "") return;
    scrollToRow(currentFile);
  }, [currentFile]);

  return (
    <Table<DataType>
      ref={tableRef}
      columns={columns}
      dataSource={data}
      pagination={false}
      className='fade-in-down grow-item'
      size='small'
      rowKey={"name"}
      scroll={{ x: "max-content" }}
      rowClassName={record => {
        const prefix = currentPath === "/" ? "" : currentPath;
        const absoluteRowPath = `${prefix}/${record.name}`;
        return currentFile.startsWith(absoluteRowPath) ? "bg-gray-200" : "";
      }}
      sticky
    />
  );
};

export default TableView;
