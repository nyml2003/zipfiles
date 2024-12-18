import React, { useCallback, useContext, useEffect, useState } from "react";
import { Button, Form, Input, Space, Tooltip } from "antd";
import useApi from "@useApi";
import { ReadConfigRequest, ReadConfigResponse } from "@/apis/ReadConfig";
import { ApiEnum } from "@/apis";
import { UpdateConfigRequest, UpdateConfigResponse } from "@/apis/UpdateConfig";
import { ReportError } from "@/stores/NotificationReducer";
import { useDispatch } from "react-redux";
import { AcceptableError } from "@/hooks/useApi/types";
import { UndoOutlined } from "@ant-design/icons";
import PathChecker from "@/components/PathChecker/PathChecker";
type Config = Partial<{
  defaultBackupPath: string; // 默认备份路径
}>;
import { Context, createProvider as createRestoreProvider } from "@/components/PathChecker/store";

const ConfigPageContent: React.FC = () => {
  const [form] = Form.useForm();
  const [initialConfig, setInitialConfig] = useState<Config | null>(null);
  const api = useApi();
  const dispatch = useDispatch();
  const { state, actions } = useContext(Context);
  const fetchData = async () => {
    api
      .request<ReadConfigRequest, ReadConfigResponse>(ApiEnum.ReadConfig, {})
      .then(res => {
        setInitialConfig(res);
        actions.updatePath(res.defaultBackupPath);
      })
      .catch(e => {
        if (!(e instanceof AcceptableError)) {
          return;
        }
        dispatch(
          ReportError({
            state: "error",
            text: "获取配置失败",
            description: (e as Error).message
          })
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
            description: (e as Error).message
          })
        );
      });
  };

  const selectedPath = useCallback(() => {
    if (state.file !== "") {
      return state.file;
    }
    if (state.path !== "") {
      return state.path;
    }
    return "";
  }, [state.path, state.file]);

  useEffect(() => {
    form.setFieldsValue({ defaultBackupPath: selectedPath() });
  }, [state.path, state.file]);

  return (
    <div
      className='fade-in-down p-4 max-w-4xl bg-white split-container-col grow-item'
      style={{
        height: "calc(100vh - 56px)",
        width: "calc(100vw - 200px)"
      }}>
      {initialConfig && (
        <Form
          labelCol={{ span: 6 }}
          wrapperCol={{ span: 16 }}
          layout='horizontal'
          variant='outlined'
          scrollToFirstError
          form={form}
          initialValues={initialConfig}
          onFinish={onFinish}>
          {
            // @ts-ignore
            zipfiles.BASE_ENV === "prod" ? (
              <Form.Item label='UI-bundle版本'>
                <Input
                  disabled
                  value={
                    // @ts-ignore
                    zipfiles.BUILD_ID
                  }
                />
              </Form.Item>
            ) : null
          }
          {
            // @ts-ignore
            zipfiles.BASE_ENV === "prod" ? (
              <Form.Item label='UI-打包时间'>
                <Input
                  disabled
                  value={
                    // @ts-ignore
                    zipfiles.BUILD_TIME
                  }
                />
              </Form.Item>
            ) : null
          }
          <Form.Item
            label='备份路径'
            name='defaultBackupPath'
            rules={[{ required: true, message: "请输入备份路径" }]}>
            <Input disabled value={selectedPath()} />
          </Form.Item>
          <Form.Item wrapperCol={{ offset: 6, span: 16 }}>
            <PathChecker />
          </Form.Item>
          <Form.Item wrapperCol={{ offset: 6, span: 16 }}>
            <Space>
              <Button type='primary' htmlType='submit'>
                更新
              </Button>
              <Tooltip title='重置表单'>
                <Button htmlType='button' onClick={onReset} icon={<UndoOutlined />}>
                  重置
                </Button>
              </Tooltip>
            </Space>
          </Form.Item>
        </Form>
      )}
    </div>
  );
};
const ConfigPage = () => {
  return createRestoreProvider(<ConfigPageContent />);
};
export default ConfigPage;
