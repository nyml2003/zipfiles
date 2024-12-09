import React, { useCallback, useEffect, useState } from "react";
import { message, Popconfirm, Space, Table, Tooltip } from "antd";
import type { TableColumnsType } from "antd";
import { ApiEnum } from "@/apis";
import useApi from "@useApi";
import { GetCommitListRequest, GetCommitListResponse } from "@/apis/GetCommitList";
import { LogicDeleteCommitRequest, LogicDeleteCommitResponse } from "@/apis/LogicDeleteCommit";
import { useDispatch } from "react-redux";
import { ReportError } from "@/stores/NotificationReducer";
import { AcceptableError } from "@/hooks/useApi/types";
import Button from "@/components/Button";
import { TableRowSelection } from "antd/lib/table/interface";
import { useNavigate } from "react-router-dom";
import { FolderUpload } from "@icon-park/react";
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
  const [selectedRowKeys, setSelectedRowKeys] = useState<React.Key[]>([]);
  const onSelectChange = (selectedRowKeys: React.Key[]) => {
    setSelectedRowKeys(selectedRowKeys);
  };
  const handleDeleteSelected = useCallback(() => {
    if (!selectedRowKeys.length) {
      message.error("请选择要删除的项");
      return;
    }
    selectedRowKeys.forEach(async key => {
      if (typeof key === "string") {
        deleteCommit(key).then(() => setData(data => data.filter(item => item.uuid !== key)));
      }
    });
  }, [selectedRowKeys]);
  const rowSelection: TableRowSelection<DataType> = {
    selectedRowKeys,
    onChange: onSelectChange,
    selections: [Table.SELECTION_ALL, Table.SELECTION_INVERT, Table.SELECTION_NONE]
  };

  const columns: TableColumnsType<DataType> = [
    {
      title: <div>操作</div>,
      dataIndex: "operation",
      key: "operation",
      render: (_, record) => (
        <Space>
          <Button
            variant='confirm'
            onClick={() => {
              if (!record.uuid) return;
              openExplorer(record.uuid);
            }}>
            查看
          </Button>
          <Button
            variant='primary'
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
            <Button variant='danger'>删除</Button>
          </Popconfirm>
        </Space>
      ),
      ellipsis: true,
      align: "center",
      fixed: "left"
    },
    {
      title: "ID",
      dataIndex: "uuid",
      key: "uuid",
      ellipsis: true,
      align: "center"
    },
    {
      title: "提交者",
      dataIndex: "author",
      key: "author",
      ellipsis: true,
      align: "center"
    },
    {
      title: "提交时间",
      dataIndex: "createTime",
      key: "createTime",
      ellipsis: true,
      align: "center",
      render: (value: number) => new Date(value * 1000).toLocaleString()
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
            whiteSpace: "nowrap"
          }}>
          {value}
        </div>
      )
    },
    {
      title: "存储路径",
      dataIndex: "storagePath",
      key: "storagePath",
      ellipsis: true,
      align: "center"
    },
    {
      title: "是否加密",
      dataIndex: "isEncrypt",
      key: "isEncrypt",
      render: (value: boolean) =>
        value ? <div style={{ color: "green" }}>√</div> : <div style={{ color: "red" }}>×</div>,
      width: 100,
      align: "center"
    }
  ];
  const fetchData = () => {
    api
      .request<GetCommitListRequest, GetCommitListResponse>(ApiEnum.GetCommitList, {})
      .then(res => {
        setData(res.commits);
      })
      .catch(e => {
        if (!(e instanceof AcceptableError)) {
          return;
        }
        dispatch(
          ReportError({
            state: "error",
            text: "获取备份列表失败",
            description: (e as Error).message
          })
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
        commitId
      }
    );
  };
  const navigate = useNavigate();
  const footer = () => {
    return (
      <div className='flex justify-end'>
        <Button variant='danger' onClick={() => handleDeleteSelected()}>
          批量删除
        </Button>
      </div>
    );
  };
  return (
    <>
      {data.length === 0 ? (
        <div className='text-center mt-4'>
          <p className='text-gray-600 mb-4'>您的备份列表空空如也，快去备份一些文件吧～</p>
          <Tooltip title='点击跳转到备份页面'>
            <Button variant='primary' onClick={() => navigate("/new_commit")}>
              <FolderUpload theme='filled' size='24' fill='#333' strokeLinecap='butt' />
            </Button>
          </Tooltip>
        </div>
      ) : (
        <Table<DataType>
          rowSelection={rowSelection}
          columns={columns}
          dataSource={data}
          pagination={false}
          className='fade-in-down rounded-lg overflow-hidden'
          size='small'
          rowKey={"uuid"}
          scroll={{ x: "max-content" }}
          footer={footer}
          sticky
        />
      )}
    </>
  );
};

export default CommitTable;
