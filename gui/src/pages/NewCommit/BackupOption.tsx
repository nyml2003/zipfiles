import { Button, Checkbox, Form, Input, message } from 'antd';
import React from 'react';
import { useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { PostBackupRequest } from '@/apis/PostBackup';
import { v4 as uuidv4 } from 'uuid';
import { findLongestCommonPrefix } from '@/utils';
type BackupFormProps = Partial<{
  message: string;
  storagePath: string;
  isEncrypt: boolean;
  author: string;
  key?: string;
}>;

const initialState: BackupFormProps = {
  isEncrypt: false,
  author: 'root',
};

const BackupOption: React.FC = () => {
  const [form] = Form.useForm();
  const expandedSelectedFile = useSelector((state: RootState) => state.createCommit.expandedSelectedFile);
  const [messageApi, contextHolder] = message.useMessage();
  const onFinish = (values: Required<BackupFormProps>) => {
    if (!expandedSelectedFile.length) {
      messageApi.error('请选择文件');
      return;
    }
    const uuid = uuidv4();
    const request: PostBackupRequest = {
      files: expandedSelectedFile,
      ...values,
      uuid,
      createTime: Date.now(),
      lca: findLongestCommonPrefix(expandedSelectedFile),
    };
    console.log(request);
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
            rules={[{ required: true, message: '请选择是否加密' }]}>
            <Checkbox />
          </Form.Item>
          <Form.Item shouldUpdate noStyle>
            {() => {
              const encrypt = form.getFieldValue('isEncrypt');
              return encrypt ? (
                <Form.Item
                  label='密码'
                  name='key'
                  rules={[
                    {
                      required: true,
                      message: '请输入密码',
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
            rules={[{ required: true, message: '请输入commit信息' }]}
            valuePropName='value'>
            <Input />
          </Form.Item>

          <Form.Item label='作者' name='author' rules={[{ required: true, message: '请输入作者' }]}>
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
