import React, { useEffect, useState } from "react";
import { Button, Form, Input, InputNumber, Space } from "antd";
import useApi from "@useApi";
import { ReadConfigRequest, ReadConfigResponse } from "@/apis/ReadConfig";
import { ApiEnum } from "@/apis";
import { UpdateConfigRequest, UpdateConfigResponse } from "@/apis/UpdateConfig";
import { ReportError } from "@/stores/NotificationReducer";
import { useDispatch } from "react-redux";
import { AcceptableError } from "@/hooks/useApi/types";
type Config = Partial<{
  ip: string; // 服务器ip
  defaultBackupPath: string; // 默认备份路径
  port: number; // 端口
  version: string; // 版本
}>;

const ConfigPage: React.FC = () => {
  const [form] = Form.useForm();
  const [initialConfig, setInitialConfig] = useState<Config | null>(null);
  const api = useApi();
  const dispatch = useDispatch();
  const fetchData = async () => {
    api
      .request<ReadConfigRequest, ReadConfigResponse>(ApiEnum.ReadConfig, {})
      .then(setInitialConfig)
      .catch(e => {
        if (!(e instanceof AcceptableError)) {
          return;
        }
        dispatch(
          ReportError({
            state: "error",
            text: "获取配置失败",
            description: (e as Error).message,
          }),
        );
      });
  };
  useEffect(() => {
    fetchData();
  }, []);

  const onReset = () => {
    form.resetFields();
  };

  const onFinish = (values: Config) => {
    const request: UpdateConfigRequest = {};
    if (values.ip && values.ip !== initialConfig?.ip) {
      request.ip = values.ip;
    }
    if (values.defaultBackupPath && values.defaultBackupPath !== initialConfig?.defaultBackupPath) {
      request.defaultBackupPath = values.defaultBackupPath;
    }
    if (Object.keys(request).length === 0) {
      return;
    }
    api
      .request<UpdateConfigRequest, UpdateConfigResponse>(ApiEnum.UpdateConfig, request)
      .then(() => {
        fetchData();
      })
      .catch(e => {
        if (!(e instanceof AcceptableError)) {
          return;
        }
        dispatch(
          ReportError({
            state: "error",
            text: "更新配置失败",
            description: (e as Error).message,
          }),
        );
      });
  };

  return (
    <div className='fade-in-down p-4 max-w-3xl'>
      {initialConfig && (
        <Form
          labelCol={{ span: 4 }}
          wrapperCol={{ span: 14 }}
          layout='horizontal'
          variant='outlined'
          scrollToFirstError
          form={form}
          initialValues={initialConfig}
          onFinish={onFinish}>
          <Form.Item
            label='服务器ip'
            name='ip'
            rules={[{ required: true, message: "请输入服务器ip" }]}>
            <Input />
          </Form.Item>

          <Form.Item
            label='默认备份路径'
            name='defaultBackupPath'
            rules={[{ required: true, message: "请输入默认备份路径" }]}>
            <Input />
          </Form.Item>

          <Form.Item label='端口' name='port'>
            <InputNumber disabled />
          </Form.Item>

          <Form.Item label='版本' name='version'>
            <Input disabled />
          </Form.Item>
          <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
            <Space>
              <Button type='primary' htmlType='submit'>
                提交
              </Button>
              <Button htmlType='button' onClick={onReset}>
                重置
              </Button>
            </Space>
          </Form.Item>
        </Form>
      )}
    </div>
  );
};

export default ConfigPage;
