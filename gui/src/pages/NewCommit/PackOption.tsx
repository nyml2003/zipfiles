//压缩，加密，浏览

import { Button, Checkbox, Form, Input } from 'antd';
import React from 'react';
import styles from './index.module.less';
import { useSelector } from 'react-redux';
import { RootState } from '@/stores/store';

const PackOption: React.FC = () => {
  const [form] = Form.useForm();
  const backupConfig = useSelector((state: RootState) => state.file.backupConfig);

  const onFinish = (values: any) => {
    console.log('Received values:', values);
  };

  const handleSumbit = async () => {
    const values = await form.validateFields();
    console.log('values', values);
  };

  return (
    <div className='flex flex-col w-full'>
      备份文件设置
      <div className={`${styles['fade-in-down']} p-4 max-w-3xl`}>
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
                  rules={[{ required: !!backupConfig.encrypt, message: '请输入密码' }]}
                  shouldUpdate>
                  <Input.Password />
                </Form.Item>
              ) : null;
            }}
          </Form.Item>
          <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
            <Button type='primary' onClick={handleSumbit}>
              提交
            </Button>
          </Form.Item>
        </Form>
      </div>
    </div>
  );
};

export default PackOption;
