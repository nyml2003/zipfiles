import { Button, Checkbox, Form, Input } from 'antd';
import React from 'react';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { updateBackupConfig } from '@/stores/file/reducer';

const BackupOption: React.FC = () => {
  const [form] = Form.useForm();
  const backupConfig = useSelector((state: RootState) => state.file.backupConfig);
  const dispatch = useDispatch();
  const onFinish = (values: typeof backupConfig) => {
    dispatch(updateBackupConfig(values));
  };

  return (
    <div className='p-2'>
      备份文件设置
      <div className='fade-in-down p-4 max-w-3xl'>
        <Form
          labelCol={{ span: 4 }}
          wrapperCol={{ span: 14 }}
          layout='horizontal'
          variant='outlined'
          scrollToFirstError
          form={form}
          initialValues={backupConfig}
          onFinish={onFinish}>
          <Form.Item
            label='是否压缩'
            name='compress'
            rules={[{ required: true, message: '请选择是否压缩' }]}
            valuePropName='checked'>
            <Checkbox />
          </Form.Item>
          <Form.Item
            label='是否加密'
            name='encrypt'
            valuePropName='checked'
            rules={[{ required: true, message: '请选择是否加密' }]}>
            <Checkbox />
          </Form.Item>
          <Form.Item shouldUpdate noStyle>
            {() => {
              const encrypt = form.getFieldValue('encrypt');
              console.log('encrypt', encrypt);
              return encrypt ? (
                <Form.Item
                  label='密码'
                  name='password'
                  rules={[
                    {
                      required: !!backupConfig.encrypt,
                      message: '请输入密码',
                    },
                  ]}
                  shouldUpdate>
                  <Input.Password />
                </Form.Item>
              ) : null;
            }}
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
