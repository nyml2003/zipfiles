import React, { useEffect, useState } from "react";
import { Button, Popconfirm, Space, Table } from "antd";
import type { TableColumnsType } from "antd";
import {
  GetCommitRecycleBinRequest,
  GetCommitRecycleBinResponse,
} from "@/apis/GetCommitRecycleBin";
import useApi from "@useApi";
import { ApiEnum } from "@/apis";
import { PhysicalDeleteCommitRequest, PhysicalDeleteCommitResponse } from "@/apis/PhysicalDeleteCommit";
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

const RecycleBin = () => {
  const api = useApi();
  const [data, setData] = useState<DataType[]>([]);
  const columns: TableColumnsType<DataType> = [
    {
      title: "操作",
      dataIndex: "operation",
      key: "operation",
      render: (_, record) => (
        <Space>
          <Popconfirm
            title={<div className='text-red-500'>删除后不可恢复，确认删除？</div>}
            onConfirm={() => {
              if (!record.uuid) return;
              deleteCommit(record.uuid);
              setData(data.filter(item => item.uuid !== record.uuid));
            }}>
            <Button danger type="primary">彻底删除</Button>
          </Popconfirm>
        </Space>
      ),
      ellipsis: true,
      align: "center",
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
      .request<GetCommitRecycleBinRequest, GetCommitRecycleBinResponse>(
        ApiEnum.GetCommitRecycleBin,
        {},
      )
      .then(res => {
        setData(res.commits);
      });
  };

  useEffect(() => {
    fetchData();
  }, []);

  const deleteCommit = async (commitId: string) => {
    await api
      .request<PhysicalDeleteCommitRequest, PhysicalDeleteCommitResponse>(ApiEnum.PhysicalDeleteCommit, {
        commitId,
      });
    
    setData(data.filter(item => item.uuid !== commitId));
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

export default RecycleBin;
