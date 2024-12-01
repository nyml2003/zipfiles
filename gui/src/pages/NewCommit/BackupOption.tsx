import { Button, Checkbox, Form, Input, message } from "antd";
import React from "react";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "@/stores/store";
import { addNotification } from "@/stores/NotificationReducer";
import { CommitPush } from "@/components/NotificationList/types";
import { handleRefresh } from "@/stores/CreateCommitReducer";
type BackupFormProps = Partial<{
  message: string;
  storagePath: string;
  isEncrypt: boolean;
  author: string;
  key?: string;
}>;

const initialState: BackupFormProps = {
  isEncrypt: false,
  author: "root",
  storagePath: "/usr/local/zipfiles",
};

const BackupOption: React.FC = () => {
  const [form] = Form.useForm();
  const dispatch = useDispatch();
  const files = useSelector((state: RootState) => state.createCommit.selectedFile.files);
  const directories = useSelector(
    (state: RootState) => state.createCommit.selectedFile.directories,
  );
  const [messageApi, contextHolder] = message.useMessage();

  const onFinish = async (values: Required<BackupFormProps>) => {
    if ((!files || files.length === 0) && (!directories || directories.length === 0)) {
      messageApi.error("请选择文件");
      return;
    }
    dispatch(
      addNotification({
        type: "commitPush",
        files,
        directories,
        options: values,
      } as CommitPush),
    );
    dispatch(handleRefresh());
    form.resetFields();
  };

  return (
    <div className='p-2'>
      备份文件设置
      {contextHolder}
      <div className='fade-in-down p-4 max-w-3xl'>
        <Form
          labelCol={{ span: 4 }}
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
                      message: "请输入密码",
                    },
                  ]}
                  shouldUpdate>
                  <Input />
                </Form.Item>
              ) : null;
            }}
          </Form.Item>
          <Form.Item
            label='commit'
            name='message'
            rules={[{ required: true, message: "请输入commit信息" }]}
            valuePropName='value'>
            <Input />
          </Form.Item>

          <Form.Item label='作者' name='author' rules={[{ required: true, message: "请输入作者" }]}>
            <Input />
          </Form.Item>
          <Form.Item
            label='存储路径'
            name='storagePath'
            rules={[{ required: true, message: "请输入存储路径" }]}>
            <Input />
          </Form.Item>
          <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
            <Button type='primary' htmlType='submit'>
              提交
            </Button>
          </Form.Item>
        </Form>
      </div>
    </div>
  );
};

export default BackupOption;
