import React, { useEffect, useState, useCallback } from "react";
import { message, Popconfirm, Space, Table } from "antd";
import type { TableColumnsType } from "antd";
import {
  GetCommitRecycleBinRequest,
  GetCommitRecycleBinResponse
} from "@/apis/GetCommitRecycleBin";
import useApi from "@useApi";
import { ApiEnum } from "@/apis";
import {
  PhysicalDeleteCommitRequest,
  PhysicalDeleteCommitResponse
} from "@/apis/PhysicalDeleteCommit";
import { useDispatch } from "react-redux";
import { ReportError } from "@/stores/NotificationReducer";
import { RecoverCommitRequest, RecoverCommitResponse } from "@/apis/RecoverCommit";
import Button from "@/components/Button";
import { AcceptableError } from "@/hooks/useApi/types";
import { TableRowSelection } from "antd/lib/table/interface";
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
  const dispatch = useDispatch();
  const [data, setData] = useState<DataType[]>([]);
  const [selectedRowKeys, setSelectedRowKeys] = useState<React.Key[]>([]);
  const onSelectChange = (selectedRowKeys: React.Key[]) => {
    setSelectedRowKeys(selectedRowKeys);
  };
  const handleDeleteSelected = useCallback(() => {
    if (!selectedRowKeys.length) {
      message.error("请选择要删除的项");
      return;
    }
    const selectedKeysCopy = [...selectedRowKeys];
    Promise.all(
      selectedKeysCopy.map(key => {
        if (typeof key === "string") {
          return deleteCommit(key);
        }
        return Promise.resolve(); // 如果 key 不是字符串，返回一个立即解决的 Promise
      })
    )
      .then(() => {
        // 所有删除操作完成后，一次性更新数据
        setData(data => data.filter(item => !selectedKeysCopy.includes(item.uuid)));
      })
      .catch(error => {
        // 处理删除过程中可能出现的错误
        message.error("删除失败：" + error.message);
      });
  }, [selectedRowKeys]);
  const rowSelection: TableRowSelection<DataType> = {
    selectedRowKeys,
    onChange: onSelectChange,
    selections: [Table.SELECTION_ALL, Table.SELECTION_INVERT, Table.SELECTION_NONE]
  };

  const handleRecoverSelected = useCallback(() => {
    if (!selectedRowKeys.length) {
      message.error("请选择要还原的项");
      return;
    }
    const selectedKeysCopy = [...selectedRowKeys];
    Promise.all(
      selectedKeysCopy.map(key => {
        if (typeof key === "string") {
          return recoverCommit(key);
        }
        return Promise.resolve(); // 如果 key 不是字符串，返回一个立即解决的 Promise
      })
    )
      .catch(error => {
        // 处理删除过程中可能出现的错误
        message.error("删除失败：" + error.message);
      });
  }, [selectedRowKeys]);

  const columns: TableColumnsType<DataType> = [
    {
      title: <div>操作</div>,
      dataIndex: "operation",
      key: "operation",
      render: (_, record) => (
        <Space>
          <Popconfirm
            title={<div className='text-red-500'>删除后不可恢复，确认删除？</div>}
            onConfirm={async () => {
              if (!record.uuid) return;
              await deleteCommit(record.uuid);
              setData(data.filter(item => item.uuid !== record.uuid));
            }}>
            <Button variant='danger'>彻底删除</Button>
          </Popconfirm>
          <Button variant='primary' onClick={() => recoverCommit(record.uuid)}>
            还原
          </Button>
        </Space>
      ),
      ellipsis: true,
      align: "center"
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
      render: (value: number) => new Date(value).toLocaleString()
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
  const fetchData = async () => {
    try {
      const res = await api.request<GetCommitRecycleBinRequest, GetCommitRecycleBinResponse>(
        ApiEnum.GetCommitRecycleBin,
        {}
      );
      setData(res.commits);
    } catch (e) {
      if (e instanceof AcceptableError) {
        dispatch(
          ReportError({
            state: "error",
            text: "获取回收站失败",
            description: (e as Error).message
          })
        );
      } else {
        // 如果 e 不是 AcceptableError，可以在这里处理其他类型的错误
        // 例如，显示一个通用的错误消息
        message.error("获取回收站失败：" + (e as Error).message);
      }
    }
  };

  useEffect(() => {
    fetchData();
  }, []);

  const deleteCommit = async (commitId: string) => {
    try {
      await api.request<PhysicalDeleteCommitRequest, PhysicalDeleteCommitResponse>(
        ApiEnum.PhysicalDeleteCommit,
        {
          commitId
        }
      );
      await fetchData();
    } catch (e) {
      if (!(e instanceof AcceptableError)) {
        return;
      }
      dispatch(
        ReportError({
          state: "error",
          text: "删除提交失败",
          description: (e as Error).message
        })
      );
    }
  };

  const recoverCommit = async (commitId: string) => {
    try {
      await api.request<RecoverCommitRequest, RecoverCommitResponse>(ApiEnum.RecoverCommit, {
        commitId
      });
      await fetchData();
    } catch (e) {
      if (!(e instanceof AcceptableError)) {
        return;
      }
      dispatch(
        ReportError({
          state: "error",
          text: "还原提交失败",
          description: (e as Error).message
        })
      );
    }
  };
  const footer = () => {
    return (
      <div className='flex justify-end space-x-2'>
        <Button
          onClick={() => {
            handleDeleteSelected();
          }}
          variant='danger'>
          批量删除
        </Button>
        <Button variant='primary' onClick={() => handleRecoverSelected()}>
          批量还原
        </Button>
      </div>
    );
  };

  return (
    <>
      {data.length > 0 ? (
        <Table<DataType>
          columns={columns}
          dataSource={data}
          rowSelection={rowSelection}
          pagination={false}
          className='fade-in-down rounded-lg'
          size='small'
          rowKey={"uuid"}
          scroll={{ x: "max-content" }}
          footer={footer}
        />
      ) : (
        <div className='text-center mt-4'>
          <p className='text-gray-600 mb-4'>您的回收站空空如也</p>
        </div>
      )}
    </>
  );
};

export default RecycleBin;
