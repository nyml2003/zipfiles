import React, { useEffect, useState } from "react";
import { Button, Popconfirm, Space, Table } from "antd";
import type { TableColumnsType } from "antd";
import { ApiEnum } from "@/apis";
import useApi from "@useApi";
import { GetCommitListRequest, GetCommitListResponse } from "@/apis/GetCommitList";
import { LogicDeleteCommitRequest, LogicDeleteCommitResponse } from "@/apis/LogicDeleteCommit";
import { useDispatch } from "react-redux";
import { ReportError } from "@/stores/NotificationReducer";
interface CommitLog {
  uuid: string;
  message: string;
  createTime: number;
  storagePath: string;
  author: string;
  isEncrypt: boolean;
  isDelete: boolean;
}
type DataType = CommitLog;

interface ExplorerProps {
  openExplorer: (uuid: string) => void;
  openRestore: (uuid: string, isEncrypt: boolean) => void;
}

const CommitTable: React.FC<ExplorerProps> = ({ openExplorer, openRestore }) => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
  const dispatch = useDispatch();
  const columns: TableColumnsType<DataType> = [
    {
      title: "操作",
      dataIndex: "operation",
      key: "operation",
      render: (_, record) => (
        <Space>
          <Button
            onClick={() => {
              if (!record.uuid) return;
              openExplorer(record.uuid);
            }}>
            查看
          </Button>
          <Button
            onClick={() => {
              if (!record.uuid) return;
              openRestore(record.uuid, record.isEncrypt);
            }}>
            还原
          </Button>
          <Popconfirm
            title='确认删除？'
            onConfirm={() => {
              if (!record.uuid) return;
              deleteCommit(record.uuid);
              setData(data.filter(item => item.uuid !== record.uuid));
            }}>
            <Button>删除</Button>
          </Popconfirm>
        </Space>
      ),
      ellipsis: true,
      align: "center",
      fixed: "left",
    },
    {
      title: "ID",
      dataIndex: "uuid",
      key: "uuid",
      ellipsis: true,
      align: "center",
    },
    {
      title: "提交者",
      dataIndex: "author",
      key: "author",
      ellipsis: true,
      align: "center",
    },
    {
      title: "提交时间",
      dataIndex: "createTime",
      key: "createTime",
      ellipsis: true,
      align: "center",
      render: (value: number) => new Date(value * 1000).toLocaleString(),
    },
    {
      title: "提交信息",
      dataIndex: "message",
      key: "message",
      ellipsis: true,
      align: "center",
      width: 100,
      render: (value: string) => (
        <div
          style={{
            maxWidth: 100,
            overflow: "hidden",
            textOverflow: "ellipsis",
            whiteSpace: "nowrap",
          }}>
          {value}
        </div>
      ),
    },
    {
      title: "存储路径",
      dataIndex: "storagePath",
      key: "storagePath",
      ellipsis: true,
      align: "center",
    },
    {
      title: "是否加密",
      dataIndex: "isEncrypt",
      key: "isEncrypt",
      render: (value: boolean) =>
        value ? <div style={{ color: "green" }}>√</div> : <div style={{ color: "red" }}>×</div>,
      width: 100,
      align: "center",
    },
  ];
  const fetchData = () => {
    api
      .request<GetCommitListRequest, GetCommitListResponse>(ApiEnum.GetCommitList, {})
      .then(res => {
        setData(res.commits);
      })
      .catch(e => {
        dispatch(
          ReportError({
            state: "error",
            text: "获取提交列表失败",
            description: (e as Error).message,
          }),
        );
      });
  };

  useEffect(() => {
    fetchData();
  }, []);

  const deleteCommit = async (commitId: string) => {
    await api.request<LogicDeleteCommitRequest, LogicDeleteCommitResponse>(
      ApiEnum.LogicDeleteCommit,
      {
        commitId,
      },
    );
  };

  return (
    <Table<DataType>
      columns={columns}
      dataSource={data}
      pagination={false}
      className='fade-in-down rounded-lg'
      size='small'
      rowKey={"uuid"}
      scroll={{ x: "max-content" }}
      sticky
    />
  );
};

export default CommitTable;
