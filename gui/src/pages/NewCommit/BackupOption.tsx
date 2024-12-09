import { Button, Checkbox, Form, Input, message } from "antd";
import React, { useCallback, useContext, useEffect, useState } from "react";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "@/stores/store";
import { addNotification, ReportError } from "@/stores/NotificationReducer";
import { CommitPush } from "@/components/NotificationList/types";
import { handleRefresh } from "@/stores/CreateCommitReducer";
import { AcceptableError } from "@/hooks/useApi/types";
import { ReadConfigRequest, ReadConfigResponse } from "@/apis/ReadConfig";
import useApi from "@/hooks/useApi/development";
import { ApiEnum } from "@/apis";
import { createProvider as createRestoreProvider, Context } from "@/components/PathChecker/store";
import PathChecker from "@/components/PathChecker/PathChecker";
type BackupFormProps = Partial<{
  message: string;
  storagePath: string;
  isEncrypt: boolean;
  author: string;
  key?: string;
}>;
type Config = Partial<{
  defaultBackupPath: string; // 默认备份路径
}>;
const initialState: BackupFormProps = {
  isEncrypt: false,
  author: "root"
};

const BackupOptionContent: React.FC = () => {
  const [form] = Form.useForm();
  const dispatch = useDispatch();
  const files = useSelector((state: RootState) => state.createCommit.selectedFile.files);
  const directories = useSelector(
    (state: RootState) => state.createCommit.selectedFile.directories
  );

  const onFinish = async (values: Required<BackupFormProps>) => {
    if ((!files || files.length === 0) && (!directories || directories.length === 0)) {
      message.error("请选择文件");
      return;
    }
    dispatch(
      addNotification({
        type: "commitPush",
        files,
        directories,
        options: values
      } as CommitPush)
    );
    dispatch(handleRefresh());
    form.resetFields();
  };
  const { state, actions } = useContext(Context);
  const api = useApi();
  const [initialConfig, setInitialConfig] = useState<Config | null>(null);

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
    form.setFieldsValue({ storagePath: selectedPath() });
  }, [state.path, state.file]);

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

  return (
    <div className='p-4'>
      <div className='text-xl font-semibold'>备份选项</div>
      <div className='fade-in-down p-4 max-w-3xl'>
        {initialConfig && (
          <Form
            labelCol={{ span: 6 }}
            wrapperCol={{ span: 14 }}
            layout='horizontal'
            variant='outlined'
            scrollToFirstError
            form={form}
            initialValues={initialState}
            onFinish={onFinish}>
            <Form.Item
              label='是否加密'
              name='isEncrypt'
              valuePropName='checked'
              rules={[{ required: true, message: "请选择是否加密" }]}>
              <Checkbox />
            </Form.Item>
            <Form.Item shouldUpdate noStyle>
              {() => {
                const encrypt = form.getFieldValue("isEncrypt");
                return encrypt ? (
                  <Form.Item
                    label='密码'
                    name='key'
                    rules={[
                      {
                        required: true,
                        message: "请输入密码"
                      }
                    ]}
                    shouldUpdate>
                    <Input />
                  </Form.Item>
                ) : null;
              }}
            </Form.Item>
            <Form.Item
              label='提交信息'
              name='message'
              rules={[{ required: true, message: "请输入提交信息" }]}
              valuePropName='value'>
              <Input />
            </Form.Item>

            <Form.Item
              label='作者'
              name='author'
              rules={[{ required: true, message: "请输入作者" }]}>
              <Input />
            </Form.Item>
            <Form.Item
              label='备份路径'
              name='storagePath'
              rules={[{ required: true, message: "请输入备份路径" }]}>
              <Input disabled value={selectedPath()} />
            </Form.Item>
            <Form.Item wrapperCol={{ offset: 6, span: 14 }}>
              <PathChecker />
            </Form.Item>
            <Form.Item wrapperCol={{ offset: 6, span: 14 }}>
              <Button type='primary' htmlType='submit'>
                提交
              </Button>
            </Form.Item>
          </Form>
        )}
      </div>
    </div>
  );
};

const BackupOption = () => {
  return createRestoreProvider(<BackupOptionContent />);
};

export default BackupOption;
